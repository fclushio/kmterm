#ifndef KMTERM_FONT_FONT_H
#define KMTERM_FONT_FONT_H

#include <linux/types.h>

#include "basic.h"

enum font_enum {
    font_min = 0,
    font_basic = font_min,
    font_max
};

struct kmterm_font_size_t {
    u16 h;
    u16 v;
};

extern const struct kmterm_font_size_t kmterm_font_size[];


#endif // KMTERM_FONT_FONT_H
