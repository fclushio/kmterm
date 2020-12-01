#include "tty.h"
#include "io.h"

#include "log.h"

#define KMTERM_TTY_INDEX 0

static int  kmterm_tty_open (struct tty_struct * _tty, struct file * _file);
static void kmterm_tty_close(struct tty_struct * _tty, struct file * _file);

static int resize(struct tty_struct *tty, struct winsize *ws)
{
    KMTERM_INF("%s: resize row:%d col:%d xp:%d yp:%d\n", tty->name, ws->ws_row, ws->ws_col, ws->ws_xpixel, ws->ws_ypixel);
    return 0;
}

static const struct tty_port_operations null_port_ops = { };


int kmterm_tty_init(const char * _name,
                    const struct kmterm_tty_ops *_config,
                    struct kmterm_tty * _tty)
{
    int err;

    _tty->driver = alloc_tty_driver(1);
    tty_port_init(&_tty->port);
    _tty->port.ops = &null_port_ops;

    if ( !_tty->driver ) {
        KMTERM_ERR("alloc_tty_driver failed : %s.\n", _name);
        return -ENOMEM;
    }

    _tty->driver->magic        = TTY_DRIVER_MAGIC;
    _tty->driver->owner        = THIS_MODULE;
    _tty->driver->driver_name  = _name;
    _tty->driver->name         = _name;
    _tty->driver->major        = 0;
    _tty->driver->num          = 1;
    _tty->driver->type         = TTY_DRIVER_TYPE_SERIAL;
    _tty->driver->subtype      = SERIAL_TYPE_NORMAL;
    _tty->driver->flags        = TTY_DRIVER_REAL_RAW | TTY_DRIVER_DYNAMIC_DEV | TTY_DRIVER_UNNUMBERED_NODE;
    _tty->driver->init_termios = tty_std_termios;
    _tty->driver->init_termios.c_cflag  = B9600 | CS8 | CREAD | HUPCL | CLOCAL;
    _tty->ops.open            = kmterm_tty_open;
    _tty->ops.close           = kmterm_tty_close;
    _tty->ops.resize          = resize;
    _tty->ops.ioctl           = _config->ioctl;
    _tty->ops.write           = _config->write;
    _tty->ops.write_room      = _config->write_room;
    _tty->ops.chars_in_buffer = _config->chars_in_buffer;

    tty_set_operations(_tty->driver, &_tty->ops);

    if ( (err = tty_register_driver(_tty->driver)) ) {
        KMTERM_ERR("%s: tty_register_driver failed : %s. Exit-code: %d.\n", _name, _name, err);
        kmterm_tty_exit(_tty);
        return err;
    }
    if ( tty_register_device(_tty->driver, KMTERM_TTY_INDEX, 0) == 0 ) {
        KMTERM_ERR("%s: tty_register_device failed [0].\n", _name);
        kmterm_tty_exit(_tty);
        return err;
    }
    tty_port_link_device(&_tty->port, _tty->driver, KMTERM_TTY_INDEX);
    KMTERM_INF("[init] tty: %s\n", _name);

    return 0;
}


void kmterm_tty_exit(struct kmterm_tty * _tty)
{
    KMTERM_INF("~[exit] tty : %s\n", _tty->driver->name);
    if ( _tty->driver ) {
        tty_unregister_device(_tty->driver, KMTERM_TTY_INDEX);
        tty_unregister_driver(_tty->driver);
        put_tty_driver(_tty->driver);
        tty_port_destroy(&_tty->port);
        _tty->driver = 0;
    }
}

static int kmterm_tty_open(struct tty_struct * _tty, struct file * _file)
{
    KMTERM_INF("%s: kmterm_tty_open\n", _tty->name);
    kmterm_io_resize();
    return tty_port_open(_tty->port, _tty, _file);
}
static void kmterm_tty_close(struct tty_struct * _tty, struct file * _file)
{
    KMTERM_INF("%s: kmterm_tty_close\n", _tty->name);
    tty_port_close(_tty->port, _tty, _file);
}










#if 0

static void set_termios(struct tty_struct *tty, struct ktermios * old)
{
    if ( old ) {
        KMTERM_INF("%s: set_termios\n", tty->name);
        KMTERM_INF("\tif 0x%08x\n", old->c_iflag);
        KMTERM_INF("\tof 0x%08x\n", old->c_oflag);
        KMTERM_INF("\tcf 0x%08x\n", old->c_cflag);
        KMTERM_INF("\tlf 0x%08x\n", old->c_lflag);
        KMTERM_INF("\tli 0x%02x\n", old->c_line );
        KMTERM_INF("-->\n");
    } else {
        KMTERM_INF("%s: set_termios NEW:\n");
    }
    KMTERM_INF("\tif 0x%08x\n", tty->termios.c_iflag);
    KMTERM_INF("\tof 0x%08x\n", tty->termios.c_oflag);
    KMTERM_INF("\tcf 0x%08x\n", tty->termios.c_cflag);
    KMTERM_INF("\tlf 0x%08x\n", tty->termios.c_lflag);
    KMTERM_INF("\tli 0x%02x\n", tty->termios.c_line );
    KMTERM_INF("\n");
}
static int install(struct tty_driver *driver, struct tty_struct *tty)
{
    KMTERM_INF("%s: install : 0x%08x --> 0x%08x\n", tty->name, tty, driver->ttys[tty->index]);
    tty->count++; // mandatory
    tty_init_termios(tty);
	tty_driver_kref_get(driver);
	driver->ttys[tty->index] = tty;
	return 0;

    /*tty->winsize.ws_row = 22;
    tty->winsize.ws_col = 15;
    tty->winsize.ws_xpixel = 8;
    tty->winsize.ws_ypixel = 14;*/
    return 0;
}
static void remove(struct tty_driver *self, struct tty_struct *tty)
{
    KMTERM_INF("%s: remove\n", tty->name);
}
static void shutdown(struct tty_struct * tty)
{
    KMTERM_INF("%s: shutdown\n", tty->name);
}
static void cleanup(struct tty_struct * tty)
{
    KMTERM_INF("%s: cleanup\n", tty->name);
}
static int put_char(struct tty_struct *tty, unsigned char ch)
{
    KMTERM_INF("%s: put_char\n", tty->name);
    return 0;
}
static void flush_chars(struct tty_struct *tty)
{
    KMTERM_INF("%s: flush_chars\n", tty->name);
}
static void set_termios(struct tty_struct *tty, struct ktermios * old)
{
    KMTERM_INF("%s: set_termios\n", tty->name);
    KMTERM_INF("\tif 0x%08x\n", old->c_iflag);
    KMTERM_INF("\tof 0x%08x\n", old->c_oflag);
    KMTERM_INF("\tcf 0x%08x\n", old->c_cflag);
    KMTERM_INF("\tlf 0x%08x\n", old->c_lflag);
    KMTERM_INF("\tli 0x%02x\n", old->c_line );
    KMTERM_INF("-->\n");
    KMTERM_INF("\tif 0x%08x\n", tty->termios.c_iflag);
    KMTERM_INF("\tof 0x%08x\n", tty->termios.c_oflag);
    KMTERM_INF("\tcf 0x%08x\n", tty->termios.c_cflag);
    KMTERM_INF("\tlf 0x%08x\n", tty->termios.c_lflag);
    KMTERM_INF("\tli 0x%02x\n", tty->termios.c_line );
    KMTERM_INF("\n");
}
static void set_ldisc(struct tty_struct *tty)
{
    KMTERM_INF("%s: set_ldisc\n", tty->name);
}
static void throttle(struct tty_struct * tty)
{
    KMTERM_INF("%s: throttle\n", tty->name);
}
static void unthrottle(struct tty_struct * tty)
{
    KMTERM_INF("%s: unthrottle\n", tty->name);
}
static void stop(struct tty_struct *tty)
{
    KMTERM_INF("%s: stop\n", tty->name);
}
static void start(struct tty_struct *tty)
{
    KMTERM_INF("%s: start\n", tty->name);
}
static void hangup(struct tty_struct *tty)
{
    KMTERM_INF("%s: hangup\n", tty->name);
}
static int resize(struct tty_struct *tty, struct winsize *ws)
{
    KMTERM_INF("%s: resize %d %d\n", tty->name, ws->ws_row, ws->ws_col);
    return 0;
}

static int install(struct tty_driver *driver, struct tty_struct *tty)
{
    KMTERM_INF("%s: install : 0x%08x --> 0x%08x\n", tty->name, tty, driver->ttys[tty->index]);
    tty->count++; // mandatory
    tty_init_termios(tty);
	tty_driver_kref_get(driver);
	driver->ttys[tty->index] = tty;
	return 0;

    /*tty->winsize.ws_row = 22;
    tty->winsize.ws_col = 15;
    tty->winsize.ws_xpixel = 8;
    tty->winsize.ws_ypixel = 14;*/
    return 0;
}

//    _tty->ops.install = install;
//    _tty->ops.remove = remove;
//    _tty->ops.shutdown = shutdown;
//    _tty->ops.cleanup = cleanup;
//    _tty->ops.put_char      = put_char;
//    _tty->ops.flush_chars   = flush_chars;
//    _tty->ops.set_termios   = set_termios;
//    _tty->ops.set_ldisc     = set_ldisc;
//    _tty->ops.throttle      = throttle;
//    _tty->ops.unthrottle    = unthrottle;
//    _tty->ops.stop          = stop;
//    _tty->ops.start = start;
//    _tty->ops.hangup = hangup;
//    _tty->ops.resize = resize;
#endif