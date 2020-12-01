#ifndef KMTERM_TERMINAL_H
#define KMTERM_TERMINAL_H

#include <linux/types.h>

#define KMTERM_MAX_CACHE_SIZE ( PAGE_SIZE  )

struct character {
    u8 back_color;
    u8 font_color;
    u8 symbol;
};

int  kmterm_terminal_init(void);
void kmterm_terminal_exit(void);

void kmterm_process_char(u8 _ch);

#endif // KMTERM_TERMINAL_H
