#ifndef KMTERM_IO_H
#define KMTERM_IO_H

/*
 * Depends on log, terminal, tty
 */

int  kmterm_io_init(void);
void kmterm_io_exit(void);
void kmterm_io_resize(void);

#endif // KMTERM_IO_H
