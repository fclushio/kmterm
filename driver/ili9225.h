#ifndef KMTERM_DRIVER_ILI9225_H
#define KMTERM_DRIVER_ILI9225_H


#include "common.h"

#define ILI9225_DRIVER_NAME "ili9225"

int  ili9225_init(void);
int ili9225_draw_character(int _h, int _v, struct character _ch);
int ili9225_draw_zone(int _h0, int _h1, int _v0, int _v1, struct character * _data);
int ili9225_clear_screen(void);

extern struct kmterm_driver_config ili9225_driver_config;


#endif // KMTERM_DRIVER_ILI9225_H
