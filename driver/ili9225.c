#include "ili9225.h"


#include <linux/delay.h>    // msleep
#include <linux/slab.h>

#define DATA KMTERM_HIGH
#define CMD  KMTERM_LOW


/* ILI9225 LCD Registers */
#define ILI9225_DRIVER_OUTPUT_CTRL      (0x01)  // Driver Output Control
#define ILI9225_LCD_AC_DRIVING_CTRL     (0x02)  // LCD AC Driving Control
#define ILI9225_ENTRY_MODE            	(0x03)  // Entry Mode
#define ILI9225_DISP_CTRL1          	(0x07)  // Display Control 1
#define ILI9225_BLANK_PERIOD_CTRL1      (0x08)  // Blank Period Control
#define ILI9225_FRAME_CYCLE_CTRL        (0x0B)  // Frame Cycle Control
#define ILI9225_INTERFACE_CTRL          (0x0C)  // Interface Control
#define ILI9225_OSC_CTRL             	(0x0F)  // Osc Control
#define ILI9225_POWER_CTRL1            	(0x10)  // Power Control 1
#define ILI9225_POWER_CTRL2           	(0x11)  // Power Control 2
#define ILI9225_POWER_CTRL3            	(0x12)  // Power Control 3
#define ILI9225_POWER_CTRL4            	(0x13)  // Power Control 4
#define ILI9225_POWER_CTRL5            	(0x14)  // Power Control 5
#define ILI9225_VCI_RECYCLING          	(0x15)  // VCI Recycling
#define ILI9225_RAM_ADDR_SET1           (0x20)  // Horizontal GRAM Address Set
#define ILI9225_RAM_ADDR_SET2           (0x21)  // Vertical GRAM Address Set
#define ILI9225_GRAM_DATA_REG           (0x22)  // GRAM Data Register
#define ILI9225_GATE_SCAN_CTRL          (0x30)  // Gate Scan Control Register
#define ILI9225_VERTICAL_SCROLL_CTRL1   (0x31)  // Vertical Scroll Control 1 Register
#define ILI9225_VERTICAL_SCROLL_CTRL2   (0x32)  // Vertical Scroll Control 2 Register
#define ILI9225_VERTICAL_SCROLL_CTRL3   (0x33)  // Vertical Scroll Control 3 Register
#define ILI9225_PARTIAL_DRIVING_POS1    (0x34)  // Partial Driving Position 1 Register
#define ILI9225_PARTIAL_DRIVING_POS2    (0x35)  // Partial Driving Position 2 Register
#define ILI9225_HORIZONTAL_WINDOW_ADDR1 (0x36)  // Horizontal Address Start Position
#define ILI9225_HORIZONTAL_WINDOW_ADDR2	(0x37)  // Horizontal Address End Position
#define ILI9225_VERTICAL_WINDOW_ADDR1   (0x38)  // Vertical Address Start Position
#define ILI9225_VERTICAL_WINDOW_ADDR2   (0x39)  // Vertical Address End Position
#define ILI9225_GAMMA_CTRL1            	(0x50)  // Gamma Control 1
#define ILI9225_GAMMA_CTRL2             (0x51)  // Gamma Control 2
#define ILI9225_GAMMA_CTRL3            	(0x52)  // Gamma Control 3
#define ILI9225_GAMMA_CTRL4            	(0x53)  // Gamma Control 4
#define ILI9225_GAMMA_CTRL5            	(0x54)  // Gamma Control 5
#define ILI9225_GAMMA_CTRL6            	(0x55)  // Gamma Control 6
#define ILI9225_GAMMA_CTRL7            	(0x56)  // Gamma Control 7
#define ILI9225_GAMMA_CTRL8            	(0x57)  // Gamma Control 8
#define ILI9225_GAMMA_CTRL9             (0x58)  // Gamma Control 9
#define ILI9225_GAMMA_CTRL10            (0x59)  // Gamma Control 10


struct kmterm_driver_config ili9225_driver_config =
{
    .gpio_required = KMTERM_DRIVER_GPIO_RST | KMTERM_DRIVER_GPIO_DC,
    .input_present = 0,
    .minimum_width = 176,
    .maximum_width = 176,
    .minimum_height = 220,
    .maximum_height = 220,
    .spi_maxspeed = 50 * 1000 * 1000
};

static inline int send_cmd(u16 _cmd, u16 _val)
{
    kmterm_gpio_dc_set(CMD);
    return 0;
}

static inline int send_data(const void * _data, size_t _count)
{
    kmterm_gpio_dc_set(DATA);
    return kmterm_spi_append(_data, _count);
}

static inline int send_data_end(void)
{
    return kmterm_spi_flush();
}

int ili9225_init(void)
{
    kmterm_gpio_rst_set(KMTERM_HIGH);
    msleep(1);
    kmterm_gpio_rst_set(KMTERM_LOW);
    msleep(10);
    kmterm_gpio_rst_set(KMTERM_HIGH);
    msleep(50);

    /* Start Initial Sequence */
	/* Set SS bit and direction output from S528 to S1 */
	kmterm_spi_c16w16(ILI9225_POWER_CTRL1, 0x0000); // Set SAP,DSTB,STB
	kmterm_spi_c16w16(ILI9225_POWER_CTRL2, 0x0000); // Set APON,PON,AON,VCI1EN,VC
	kmterm_spi_c16w16(ILI9225_POWER_CTRL3, 0x0000); // Set BT,DC1,DC2,DC3
	kmterm_spi_c16w16(ILI9225_POWER_CTRL4, 0x0000); // Set GVDD
	kmterm_spi_c16w16(ILI9225_POWER_CTRL5, 0x0000); // Set VCOMH/VCOML voltage
	msleep(40);

	// Power-on sequence
	kmterm_spi_c16w16(ILI9225_POWER_CTRL2, 0x0018); // Set APON,PON,AON,VCI1EN,VC
	kmterm_spi_c16w16(ILI9225_POWER_CTRL3, 0x6121); // Set BT,DC1,DC2,DC3
	kmterm_spi_c16w16(ILI9225_POWER_CTRL4, 0x006F); // Set GVDD   /*007F 0088 */
	kmterm_spi_c16w16(ILI9225_POWER_CTRL5, 0x495F); // Set VCOMH/VCOML voltage
	kmterm_spi_c16w16(ILI9225_POWER_CTRL1, 0x0800); // Set SAP,DSTB,STB
	msleep(10);
	kmterm_spi_c16w16(ILI9225_POWER_CTRL2, 0x103B); // Set APON,PON,AON,VCI1EN,VC
	msleep(50);

	kmterm_spi_c16w16(ILI9225_DRIVER_OUTPUT_CTRL,  0x011C); // set the display line number and display direction
	kmterm_spi_c16w16(ILI9225_LCD_AC_DRIVING_CTRL, 0x0100); // set 1 line inversion
	kmterm_spi_c16w16(ILI9225_ENTRY_MODE,          0x1030); // set GRAM write direction and BGR=1.
	kmterm_spi_c16w16(ILI9225_DISP_CTRL1,          0x0000); // Display off
	kmterm_spi_c16w16(ILI9225_BLANK_PERIOD_CTRL1,  0x0808); // set the back porch and front porch
	kmterm_spi_c16w16(ILI9225_FRAME_CYCLE_CTRL,    0x1100); // set the clocks number per line
	kmterm_spi_c16w16(ILI9225_INTERFACE_CTRL,      0x0000); // CPU interface
	kmterm_spi_c16w16(ILI9225_OSC_CTRL,            0x0D01); // Set Osc  /*0e01*/
	kmterm_spi_c16w16(ILI9225_VCI_RECYCLING,       0x0020); // Set VCI recycling
	kmterm_spi_c16w16(ILI9225_RAM_ADDR_SET1,       0x0000); // RAM Address
	kmterm_spi_c16w16(ILI9225_RAM_ADDR_SET2,       0x0000); // RAM Address

	/* Set GRAM area */
	kmterm_spi_c16w16(ILI9225_GATE_SCAN_CTRL,          0x0000);
	kmterm_spi_c16w16(ILI9225_VERTICAL_SCROLL_CTRL1,   0x00DB);
	kmterm_spi_c16w16(ILI9225_VERTICAL_SCROLL_CTRL2,   0x0000);
	kmterm_spi_c16w16(ILI9225_VERTICAL_SCROLL_CTRL3,   0x0000);
	kmterm_spi_c16w16(ILI9225_PARTIAL_DRIVING_POS1,    0x00DB);
	kmterm_spi_c16w16(ILI9225_PARTIAL_DRIVING_POS2,    0x0000);
	kmterm_spi_c16w16(ILI9225_HORIZONTAL_WINDOW_ADDR1, 0x00AF);
	kmterm_spi_c16w16(ILI9225_HORIZONTAL_WINDOW_ADDR2, 0x0000);
	kmterm_spi_c16w16(ILI9225_VERTICAL_WINDOW_ADDR1,   0x00DB);
	kmterm_spi_c16w16(ILI9225_VERTICAL_WINDOW_ADDR2,   0x0000);

	/* Set GAMMA curve */
	kmterm_spi_c16w16(ILI9225_GAMMA_CTRL1,  0x0000);
	kmterm_spi_c16w16(ILI9225_GAMMA_CTRL2,  0x0808);
	kmterm_spi_c16w16(ILI9225_GAMMA_CTRL3,  0x080A);
	kmterm_spi_c16w16(ILI9225_GAMMA_CTRL4,  0x000A);
	kmterm_spi_c16w16(ILI9225_GAMMA_CTRL5,  0x0A08);
	kmterm_spi_c16w16(ILI9225_GAMMA_CTRL6,  0x0808);
	kmterm_spi_c16w16(ILI9225_GAMMA_CTRL7,  0x0000);
	kmterm_spi_c16w16(ILI9225_GAMMA_CTRL8,  0x0A00);
	kmterm_spi_c16w16(ILI9225_GAMMA_CTRL9,  0x0710);
	kmterm_spi_c16w16(ILI9225_GAMMA_CTRL10, 0x0710);

	kmterm_spi_c16w16(ILI9225_DISP_CTRL1, 0x0012);
	msleep(50);
	kmterm_spi_c16w16(ILI9225_DISP_CTRL1, 0x1017);

    msleep(100);

    ili9225_clear_screen();

    return 0;
}


/*
 * Note: It must be guaranteed by the caller, that according to the current
 * sysfs font config virtual horizontal character position 'h' and vertical
 * positiong 'v' do exist (in specified orientation).
 */
inline int ili9225_draw_character(int _h, int _v, struct character _ch)
{
    return ili9225_draw_zone(_h, _h + 1, _v, _v + 1, &_ch);
}

int ili9225_draw_zone(int _h0, int _h1,
                      int _v0, int _v1,
                      struct character * _data)
{
    const u16 h_pix = kmterm_font_size[ kmterm_config_sysfs.font ].h,
              v_pix = kmterm_font_size[ kmterm_config_sysfs.font ].v;
    u16 x0, x1, y0, y1;
    if ( kmterm_config_sysfs.orientation == 0 ) { // horizontal
        x0 = _h0 * h_pix    ; y0 = _v0 * v_pix;
        x1 = _h1 * h_pix - 1; y1 = _v1 * v_pix - 1;

    } else { // vertical
        u16 x_size = kmterm_config_dt.display_width;
        x0 = x_size - _v1 * v_pix    ; y0 = _h0 * h_pix;
        x1 = x_size - _v0 * v_pix - 1; y1 = _h1 * h_pix - 1;

    }

    // Select character window & place cursor in (0;0)
    kmterm_spi_c16w16(ILI9225_HORIZONTAL_WINDOW_ADDR1, x1);
	kmterm_spi_c16w16(ILI9225_HORIZONTAL_WINDOW_ADDR2, x0);
	kmterm_spi_c16w16(ILI9225_VERTICAL_WINDOW_ADDR1,   y1);
	kmterm_spi_c16w16(ILI9225_VERTICAL_WINDOW_ADDR2,   y0);
	kmterm_spi_c16w16(ILI9225_RAM_ADDR_SET1,           x0);
	kmterm_spi_c16w16(ILI9225_RAM_ADDR_SET2,           y0);

    // Write raw data command + write data
    kmterm_spi_c16(0x0022);

    if ( kmterm_config_sysfs.orientation == 0 )  {
        for ( int i = 0; i < (_v1 - _v0) * v_pix; i++ ) { // for each pixel in V
            for ( int j = 0; j < (_h1 - _h0) * h_pix; j++ ) {
                struct character ch_s = _data[(i / v_pix)*(_h1 - _h0)+ (j / h_pix)];
                //const u16 font_color = kmterm_color_table[ kmterm_color_white ];
                //const u16 back_color = kmterm_color_table[ kmterm_color_black ];
                char row = kmterm_font_basic // TODO custom font
                        [ ch_s.symbol ]
                        [ i % v_pix ];
                // TODO custom color
                if ( ( row >> (8 - (j % 8 + 1)) ) & 0x1 )
                    send_data( &kmterm_color_table[ch_s.font_color],
                               sizeof( kmterm_color_table[ch_s.font_color] ) );
                else
                    send_data( &kmterm_color_table[ch_s.back_color],
                               sizeof( kmterm_color_table[ch_s.back_color] ) );
            }
        }
    } else {
        for ( int i = 0; i < (_h1 - _h0) * h_pix; i++ ) { // for each pixel in V
            for ( int j = 0; j < (_v1 - _v0) * v_pix; j++ ) {
                struct character ch_s = _data[( ((_v1 - _v0)*v_pix - j - 1) / v_pix )*(_h1-_h0) + (i / h_pix)];
                //const u16 font_color = kmterm_color_table[ kmterm_color_white ];
                //const u16 back_color = kmterm_color_table[ kmterm_color_black ];
                char row = kmterm_font_basic
                        [ ch_s.symbol ]
                        [ v_pix - 1 - j % v_pix ];
;
                // TODO custom color
                if ( ( row >> (8 - (i % 8 + 1)) ) & 0x1 )
                    send_data( &kmterm_color_table[ch_s.font_color],
                               sizeof( kmterm_color_table[ch_s.font_color] ) );
                else
                    send_data( &kmterm_color_table[ch_s.back_color],
                               sizeof( kmterm_color_table[ch_s.back_color] ) );
            }
        }
    }
    send_data_end();

    return 0;
}

int ili9225_clear_screen(void)
{
    int n = kmterm_config_dt.display_height * kmterm_config_dt.display_width;
    u16 color = kmterm_color_table[ kmterm_color_black ]; // TODO custom color scheme

    kmterm_spi_c16w16(ILI9225_HORIZONTAL_WINDOW_ADDR1, 0x00AF);
	kmterm_spi_c16w16(ILI9225_HORIZONTAL_WINDOW_ADDR2, 0x0000);
	kmterm_spi_c16w16(ILI9225_VERTICAL_WINDOW_ADDR1,   0x00DB);
	kmterm_spi_c16w16(ILI9225_VERTICAL_WINDOW_ADDR2,   0x0000);
	kmterm_spi_c16w16(ILI9225_RAM_ADDR_SET1,           0x0000);
	kmterm_spi_c16w16(ILI9225_RAM_ADDR_SET2,           0x0000);

    kmterm_spi_c16(0x0022);
    for ( int i = 0; i < n ; i++ )
        send_data( &color, sizeof(u16) );
    send_data_end();
    return 0;
}
