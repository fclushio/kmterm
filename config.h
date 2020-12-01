#ifndef KMTERM_CONFIG_H
#define KMTERM_CONFIG_H

#include <linux/of.h> // of_find_node_by_name, of_property_read_s32, ..

#define KMTERM_DT_NODE_TITLE "kmterm"

/*
 * Depends on log module.
 */
int  kmterm_config_init(void);
void kmterm_config_exit(void);


// Callbacks
extern void (*kmterm_font_callback)       (int _font);
extern void (*kmterm_color_callback)      (int _color);
extern void (*kmterm_orientation_callback)(int _orientation);



enum kmterm_color_t {
    kmterm_color_min = 0,
    kmterm_color_black = kmterm_color_min,
    kmterm_color_red,
    kmterm_color_green,
    kmterm_color_yellow,
    kmterm_color_blue,
    kmterm_color_magenta,
    kmterm_color_cyan,
    kmterm_color_white,
    kmterm_color_bright_black,
    kmterm_color_bright_red,
    kmterm_color_bright_green,
    kmterm_color_bright_yellow,
    kmterm_color_bright_blue,
    kmterm_color_bright_magenta,
    kmterm_color_bright_cyan,
    kmterm_color_bright_white,
    kmterm_color_max
};
extern const u16 kmterm_color_table[];



struct kmterm_dt_config_t {
    const char * title;
    u32 spi_maxspeed;
    int spi_busnum;
    int spi_cs;
    int spi_mode;

    const char * driver;
    int display_width;
    int display_height;
    int gpio_dc;
    int gpio_rst;
};

struct kmterm_sysfs_config_t {
    int font;
    int color;
    int orientation;
    int log_level;
};

/*
 * This is global variable with all settings that will be used.
 */
extern struct kmterm_dt_config_t    kmterm_config_dt;
extern struct kmterm_sysfs_config_t kmterm_config_sysfs;

#endif // KMTERM_CONFIG_H
