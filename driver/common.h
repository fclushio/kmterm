#ifndef KMTERM_DRIVER_COMMON_H
#define KMTERM_DRIVER_COMMON_H

#define KMTERM_DRIVER_GPIO_RST 0x01
#define KMTERM_DRIVER_GPIO_DC  0x02 // aka RS

#include <linux/types.h>

#include "../log.h"
#include "../spi.h"
#include "../config.h"
#include "../terminal.h"
#include "../font/font.h"

struct kmterm_driver_config {
    u8 gpio_required;
    u8 input_present; // 1 -- we can read data, 0 -- only output possible
    u16 minimum_width;
    u16 maximum_width;
    u16 minimum_height;
    u16 maximum_height;
    u32 spi_maxspeed;
};

extern int kmterm_spi_data(const void *_data, int _count);

extern int  kmterm_spi_c16w16(u16 _com, u16 _val);
extern int  kmterm_spi_c16   (u16 _com);
extern int  kmterm_spi_w16   (u16 _val);

#endif // KMTERM_DRIVER_COMMON_H
