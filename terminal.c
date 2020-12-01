#include "terminal.h"
#include "driver.h"
#include "log.h"
#include "io.h"

#include <linux/slab.h>



static u16 h_size = 0;
static u16 v_size = 0;
static u16 g_v_size = 0;
static u16 h_idx = 0;
static u16 v_idx = 0;
static u16 g_v_idx = 0;
static struct character * cache = 0;
static struct character * window = 0;
static u32 page_size = 0;
static u32 cache_size = 0;

/*
 * Character struct -- represents a single character on a terminal display.
 * @back_color -- Color from an color enum, represents background color
 * @font_color -- Color from an color enum, represents font color
 * @symbol -- Symbol from ASCII
 */

static int font_color;
static int back_color;


static void map_cache(void)
{
    // Save previous values
    u16 old_h_size = h_size, old_v_size = v_size,
            old_g_v_idx = g_v_idx, old_h_idx = h_idx, old_v_idx = v_idx;
    struct character * data;
    //KMTERM_INF("OLD: h_s %d, v_s %d, gv_idx %d, hdx %d, vidx %d", old_h_size, old_v_size, old_g_v_idx, old_h_idx, old_v_idx);

    // Init terminala parameters
    if ( kmterm_config_sysfs.orientation == 0 ) {

        h_size = kmterm_config_dt.display_width /
                kmterm_font_size[ kmterm_config_sysfs.font ].h;
        v_size = kmterm_config_dt.display_height /
                kmterm_font_size[ kmterm_config_sysfs.font ].v;
    } else {

        h_size = kmterm_config_dt.display_height /
                kmterm_font_size[ kmterm_config_sysfs.font ].h;
        v_size = kmterm_config_dt.display_width /
                kmterm_font_size[ kmterm_config_sysfs.font ].v;
    }
    page_size = h_size * v_size;
    cache_size = ( KMTERM_MAX_CACHE_SIZE / h_size )
            * h_size * sizeof(struct character);
    //KMTERM_INF("%u CACHE SIZE", cache_size);
    g_v_size = cache_size / h_size / sizeof(struct character);
    //KMTERM_INF("h_size %u, v_size %u, g_v_size % u", h_size, v_size, g_v_size);

    data = kzalloc(cache_size, GFP_KERNEL);

    // If we're modifying terminal resolution
    if ( old_h_size * old_v_size ) {
        // Save old data to new buffer & init indexes
        u16 h = (h_size > old_h_size) ? old_h_size : h_size;
        u16 v = (old_g_v_idx + old_v_size > g_v_size)
                ? g_v_size : old_g_v_idx + old_v_size;
        u16 v_shift = (old_g_v_idx + old_v_size > g_v_size)
                ? (old_g_v_idx + old_v_size - g_v_size) : 0;
        //KMTERM_INF("h %u, v %u, shift %u", h,v,v_shift);
        for ( int i = 0; i < v; ++i ) {
            for ( int j = 0 ; j < h; ++j ) {
                data[i*h_size + j] = cache[(v_shift + i)*old_h_size + j];
                //KMTERM_DBG("copying %d %d --> %d %d [%d]", i, j, v_shift + i, j, cache[(v_shift + i)*old_h_size + j].symbol);
            }
        }
        h_idx = old_h_idx;
        v_idx = (v_size - (old_v_size - old_v_idx) > 0 ) ? v_size - (old_v_size - old_v_idx) : 0;
        g_v_idx = old_g_v_idx - v_shift;
        window = data + (g_v_idx - v_idx) * h_size;
        //KMTERM_INF("0x%08x 0x%08x :: %d", data, window, (g_v_idx - v_idx) * v_size);
        //KMTERM_INF("h_idx %u, v_idx %u, g_v_idx %u", h_idx,v_idx,g_v_idx);
        kfree(cache);
    } else {
        // Start from the scratch
        h_idx = 0;
        v_idx = 0;
        g_v_idx = 0;
        window = data;
    }
    cache = data;

    //page_size = h_size * v_size;
}


void on_font_change(int _font)
{
    // Full redraw with cache remap
    map_cache();
    kmterm_clear_screen();
    kmterm_draw_zone(0, h_size, 0, v_size, window);
}

void on_color_change(int _color)
{
    // Only repaint window
    kmterm_clear_screen();
    kmterm_draw_zone(0, h_size, 0, v_size, window);
}

void on_orientation_change(int _orientation)
{
    // Full redraw with cache remap
    map_cache();
    kmterm_clear_screen();
    kmterm_draw_zone(0, h_size, 0, v_size, window);
}


static int csi_pos = 0;
static u8 is_esc = 0;
static u16 csi_size = 16;
static u8 * csi_params = 0; // buffer for CSI params, dynamic growth

int kmterm_terminal_init(void)
{
    cache = kzalloc(KMTERM_MAX_CACHE_SIZE, GFP_KERNEL);
    if ( !cache ) {
        KMTERM_ERR("kzalloc failed");
        return -ENOMEM;
    }
    map_cache();
    kmterm_font_callback = on_font_change;
    kmterm_color_callback = on_color_change;
    kmterm_orientation_callback = on_orientation_change;

    csi_params = kzalloc(csi_size, GFP_KERNEL);
    font_color = kmterm_color_white;
    back_color = kmterm_color_black;
    return 0;
}

void kmterm_terminal_exit(void)
{
    kfree(csi_params);
    kfree(cache);
}


#define CSI_CUP  0x48
#define CSI_EL   0x4B
#define CSI_ED   0x4A
#define CSI_MODE 0x68
#define CSI_SGR  0x6D

static const u16 sgr_font_offset = 30 - kmterm_color_black;
static const u16 sgr_back_offset = 40 - kmterm_color_black;
static const u16 sgr_bold_offset = kmterm_color_bright_black;
static u8 sgr_bold = 0;
static u8 sgr_reverse = 0;

static void process_cup(void)
{
    int res, code, consumed;
    u16 pos[2], idx = 0;
    u8 * ptr;


    csi_params[csi_pos] = ';'; // We will always get N x "%d;"
    ptr = csi_params + 1;

    while ( idx < 2 ) {
        if ( (res = sscanf(ptr, "%d;%n", &code, &consumed)) ) {
            pos[idx] = code - 1;
        } else {
            sscanf(ptr, ";%n", &consumed);
            if ( consumed == 1 ) {
                pos[idx] = 0;
            } else {
                KMTERM_ERR("[%d] CUP command ;  Invallid params: %s",  idx, csi_params );
                return;
            }
        }
        idx++;
        ptr += consumed;
    }

//    g_v_idx += (pos[0] - v_idx);
    KMTERM_WRN("CUP command ; params: %s; (%u %u) --> (%u %u) ==> (%u %u)",
               csi_params, v_idx, h_idx, pos[0], pos[1],
            pos[0] > v_size ? v_size - 1 : pos[0],
            pos[1] > h_size ? h_size - 1 : pos[1]);
    v_idx = pos[0] > v_size ? v_size - 1 : pos[0];
    h_idx = pos[1] > h_size ? h_size - 1 : pos[1];
}

static void process_ed(void)
{
    int res, num, consumed;
    u8 * ptr;

    ptr = csi_params + 1;
    if ( (res = sscanf(ptr, "%d%n", &num, &consumed)) != 1 )
        num = 0;


    switch (num) {
    case 0:
        for ( int i = v_idx; i < v_size; ++ i )
            for ( int j = 0; j < h_size; ++ j ) {
                if ( i == v_idx )
                    j = h_idx;
                window[i * h_size + j].symbol = ' ';
                kmterm_draw_character( j, i, window[i * h_size + j]);
            }
        KMTERM_WRN("ED (\'J\' 0x4A) : %d", num);
        kmterm_draw_zone(0, h_size, 0, v_size, window);
        break;

    case 1:
        for ( int i = 0; i <= v_idx; ++ i )
            for ( int j = 0; j < h_size; ++ j ) {
                if ( i == v_idx && j > h_idx)
                    break;
                window[i * h_size + j].symbol = ' ';
            }
        KMTERM_WRN("ED (\'J\' 0x4A) : %d", num);
        kmterm_draw_zone(0, h_size, 0, v_size, window);
        break;

    case 2:
    case 3:
        for ( int i = 0; i < v_size; ++ i )
            for ( int j = 0; j < h_size; ++ j )
                window[i * h_size + j].symbol = ' ';
        KMTERM_WRN("ED (\'J\' 0x4A) : %d", num);
        kmterm_draw_zone(0, h_size, 0, v_size, window);
        break;

    default:
        KMTERM_ERR("[1] Unsupported EL code : %d", num);
        break;
    }

}

static void process_el(void)
{
    int res, num, consumed;
    u8 * ptr;

    ptr = csi_params + 1;
    if ( (res = sscanf(ptr, "%d%n", &num, &consumed)) != 1 )
        num = 0;

    switch (num) {
    case 0:
        for ( int i = h_idx; i < h_size; ++ i )
            window[v_idx * h_size + i].symbol = ' ';
        KMTERM_WRN("EL (\'K\' 0x4B) : %d", num);
        kmterm_draw_zone(0, h_size, 0, v_size, window);
        break;
    case 1:
        for ( int i = 0; i <= h_idx; ++ i )
            window[v_idx * h_size + i].symbol = ' ';
        KMTERM_WRN("EL (\'K\' 0x4B) : %d", num);
        kmterm_draw_zone(0, h_size, 0, v_size, window);
        break;
    case 2:
        for ( int i = 0; i <= h_size; ++ i )
            window[v_idx * h_size + i].symbol = ' ';
        KMTERM_WRN("EL (\'K\' 0x4B) : %d", num);
        kmterm_draw_zone(0, h_size, 0, v_size, window);
        break;

    default:
        KMTERM_ERR("EL (\'K\' 0x4B) : Unsupported %d", num);
        break;
    }

}

static void process_mode(void)
{
    int res, num, consumed;
    u8 * ptr;

    ptr = csi_params + 1;
    if ( (res = sscanf(ptr, "%d%n", &num, &consumed)) != 1 )
        num = 0;

    KMTERM_WRN("MODE (\'h\' 0x68); %s : %d",  csi_params, num );

}

static void process_sgr(void)
{
    int res, code, consumed;
    u16 temp;
    u8 * ptr;


    csi_params[csi_pos] = ';'; // We will always get N x "%d;"
    ptr = csi_params + 1;
    KMTERM_WRN("SGR command ; params: %s",  ptr );

    if ( csi_pos == 1 ) { // ESC [m == ESC [0m
        font_color = kmterm_color_white;
        back_color = kmterm_color_black;
        sgr_bold = 0;
        return;
    }

    while ( (res = sscanf(ptr, "%d;%n", &code, &consumed)) == 1 ) {
        if ( code < 0 || code > 107 ) {
            KMTERM_ERR("[0] Unsupported SGR code : %d", code);
            return;
        }
        switch ( code ) {
        case 0:
            font_color = kmterm_color_white;
            back_color = kmterm_color_black;
            sgr_bold = 0;
            if ( sgr_reverse ) {
                temp = font_color;
                font_color = back_color;
                back_color = temp;
                sgr_reverse = 0;
            }
            break;

        case 1:
            sgr_bold = 1;
            break;

        case 7:
            sgr_reverse = 1;
            temp = font_color;
            font_color = back_color;
            back_color = temp;
            break;

        case 30 ... 37:
            if ( sgr_bold ) {
                if ( sgr_reverse )
                    back_color = code - sgr_font_offset + sgr_bold_offset;
                else
                    font_color = code - sgr_font_offset + sgr_bold_offset;
            } else {
                if ( sgr_reverse )
                    back_color = code - sgr_font_offset;
                else
                    font_color = code - sgr_font_offset;
            }
            break;

        case 40 ... 47:
            if ( sgr_reverse )
                font_color = code - sgr_back_offset;
            else
                back_color = code - sgr_back_offset;
            break;

        default:
            KMTERM_ERR("[1] Unsupported SGR code : %d", code);
            break;

        }

        ptr += consumed;
    }
}



static inline void csi_finish(u8 _final)
{
    switch (_final) {
    case CSI_CUP:
        process_cup();
    case CSI_ED:
        //process_ed();
        break;
    case CSI_EL:
        //process_el();
        break;
    case CSI_MODE:
        process_mode();
        break;
    case CSI_SGR:
        process_sgr();
        break;
    default:
        KMTERM_INF("process command \'0x%x\'; params: %s..Not supported", _final, csi_params );
        return;
    }
}

static inline void csi_process(u8 _ch)
{
    if ( csi_pos == 0 ) { // CSI must start with '['
        if ( _ch != 0x5B ) {
            KMTERM_WRN("Only CSI is supported. Omitting ESC + 0x%x character.", _ch);
            is_esc = 0;
        } else {
            csi_params[csi_pos] = _ch;
            csi_pos ++;
        }
        return;
    }
    if ( csi_size <= csi_pos ) { // dynamic growth
        u8 * data = kzalloc(csi_size * 2, GFP_KERNEL);
        memcpy(data, csi_params, csi_size);
        kfree(csi_params);
        csi_size *= 2;
        if ( csi_size > PAGE_SIZE )
            KMTERM_ERR("CSI param size is growing past %d.", csi_size);
        csi_params = data;
    }
    if ( (_ch >= 0x30 && _ch <= 0x3F) ||
         (_ch >= 0x20 && _ch <= 0x2F)  )
    {
        csi_params[csi_pos] = _ch;
        csi_pos++;
        return;
    }
    if ( _ch >= 0x40 && _ch <= 0x7E ) {
        csi_finish(_ch);
        memset(csi_params, 0, csi_size);
        is_esc = 0;
        csi_pos = 0;
        return;
    }
    KMTERM_ERR("Bad CSI character: 0x%x", _ch);
    is_esc = 0;
}

static inline void do_new_line(void)
{
    //KMTERM_ERR("((((((((( NEW LINE ))))))))))");
    v_idx++;
    g_v_idx++;
    h_idx = 0;
    if ( v_idx == v_size ) {
        window = window + h_size; // add 1 row to pointer
        // if window overflow ...
        if ( window + page_size >= cache + cache_size / sizeof(*cache) ) {
            u32 offset = (cache_size/h_size) / 2 * h_size;
//            KMTERM_INF("%u CACHE SIZE, v_idx %d, g_v_idx %d", cache_size, v_idx, g_v_idx);
//            KMTERM_INF("cache size : %u, page_size %u, offset %u, h_size %u", cache_size, page_size, offset, h_size);
//            KMTERM_INF("-----===========================================================");
            memcpy(cache, window + page_size - offset/sizeof(*cache), offset);
            memset(cache + offset/sizeof(*cache), 0, cache_size - offset);
            window = cache + offset/sizeof(*cache) - (v_size - 1)*h_size;
            g_v_idx = (cache_size/h_size) / 2 / sizeof(*cache);
//            KMTERM_INF("...%u CACHE SIZE, v_idx %d, g_v_idx %d", cache_size, v_idx, g_v_idx);

        }
        kmterm_draw_zone(0, h_size, 0, v_size, window);
        v_idx--;
    }
//    KMTERM_INF("local: %d, global : %d", v_idx,g_v_idx);
}

/* Calling to this function must be synchronized */
void kmterm_process_char(u8 _ch)
{
    if ( is_esc ) {
        csi_process(_ch);
        return;
    }
    switch ( _ch ) {
    case 0x1B: // Escape sequence start
        is_esc = 1;
        return;
    case 0x8: // backspace
        if ( h_idx == 0 ) {
            h_idx = h_size;
            v_idx--;
            g_v_idx--;
        }
        h_idx --;
        if ( v_idx == 0 ) // shift up by 1 row
            (void)h_idx;
        window[v_idx * h_size + h_idx].symbol = ' ';
        kmterm_draw_character( h_idx, v_idx, window[v_idx * h_size + h_idx]);
        return;

    case 0xA: //
        do_new_line();
        return;

    case 0xD: // \r
        h_idx = 0;
        return;

    case 32 ... 126:
        /*if ( h_idx == h_size )
            do_new_line();*/
        window[v_idx * h_size + h_idx].symbol = _ch;
        window[v_idx * h_size + h_idx].back_color = back_color;
        window[v_idx * h_size + h_idx].font_color = font_color;
        kmterm_draw_character( h_idx, v_idx, window[v_idx * h_size + h_idx] );
        if ( h_idx != h_size )
            h_idx++;
        return;


    default:
        if ( _ch > 127 ) {
            kmterm_process_char('?');
            KMTERM_WRN("process_char: unhandled character : \'%u\'", _ch);
        } else {
            KMTERM_ERR("process_char: unhandled character : \'%u\'", _ch);
        }
    }
}
