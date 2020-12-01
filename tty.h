#ifndef KMTERM_TTY_H
#define KMTERM_TTY_H

#include "config.h"
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/tty_driver.h>

/*
 * This structure represents ops for a tty driver. After being passed to the
 * init() function, structure could be safely removed.
 */
struct kmterm_tty_ops {
    int (*write)          (struct tty_struct * _tty,
                           const unsigned char * _buf, int _count);
    int (*write_room)     (struct tty_struct * _tty);
    int (*chars_in_buffer)(struct tty_struct * _tty);
    int (*ioctl)          (struct tty_struct * _tty,
                           unsigned int _cmd, unsigned long _arg);
    int (*open)           (struct tty_struct * _tty, struct file * _file);
};

/*
 * This struct represents a single linux tty driver with one device linked to
 * one port. This structure must live as long as the driver is registered and
 * it is safe to remove it only after exit() function.
 */
struct kmterm_tty {
    struct tty_driver * driver;
    struct tty_operations ops;
    struct tty_port port;
};


/*
 * Depends on log, config.
 * Name pointer must be a persistent one and must live as long as driver does.
 */
int  kmterm_tty_init(const char * _name,
                     const struct kmterm_tty_ops * _config,
                     struct kmterm_tty * _tty);

void kmterm_tty_exit(struct kmterm_tty * _tty);


#endif // KMTERM_TTY_H
