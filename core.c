#include <linux/module.h>   // basic kernel module functionality

#include "log.h"
#include "config.h"
#include "driver.h"
#include "spi.h"
#include "io.h"
#include "tty.h"
#include "terminal.h"




static int kmterm_init(void);
static void kmterm_exit(void);


static int __init kmterm_kernel_module_init(void)
{
    return kmterm_init();
}

static void __exit kmterm_kernel_module_exit(void) {
    kmterm_exit();
}


static int kmterm_init(void)
{
    int err;
    do {
        if ( (err = kmterm_log_init()      ) )
            break;
        if ( (err = kmterm_config_init()   ) )
            break;
        if ( (err = kmterm_driver_init()   ) )
            break;
        if ( (err = kmterm_spi_init()      ) )
            break;
        if ( (err = kmterm_device_init()   ) )
            break;
        if ( (err = kmterm_terminal_init() ) )
            break;
        if ( (err = kmterm_io_init()       ) )
            break;
        KMTERM_MESG("KMTerm has successfully started.\n");
        return 0;
    } while ( 0 );

    kmterm_exit();
    return err;
}

static void kmterm_exit(void)
{
    kmterm_io_exit();
    kmterm_terminal_exit();
    kmterm_spi_exit();
    kmterm_config_exit();
    KMTERM_MESG("~KMTerm has successfully exited.\n");
}


module_init( kmterm_kernel_module_init );
module_exit( kmterm_kernel_module_exit );

MODULE_LICENSE     ("GPL");
MODULE_AUTHOR      ("astrekoz");
MODULE_DESCRIPTION ("kmterm LCD SPI driver (kmterm9225)");
MODULE_VERSION     ("0.01");