#ifndef KMTERM_DRIVER_H
#define KMTERM_DRIVER_H

#include "config.h"
#include "driver/ili9225.h"

u32 kmterm_driver_get_spi_speed(void);
/*
 * Depends on log, config.
 */
int kmterm_driver_init(void);
/*
 * Depends on log, config, SPI.
 */
int kmterm_device_init(void);

int kmterm_draw_character(u16 _h, u16 _v, struct character _ch);
int kmterm_draw_zone(int _h0, int _h1, int _v0, int _v1, struct character * _data);
int kmterm_clear_screen(void);


#endif // KMTERM_DRIVER_H
