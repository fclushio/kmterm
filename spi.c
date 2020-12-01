#include "spi.h"

#include "log.h"
#include "config.h"
#include "driver.h"


#include <linux/spi/spi.h>
#include <linux/gpio.h>


/*
 *
 * Static data
 *
 */

// Indicates pin value HIGH, LOW or HIZ
static u8 * buffer;

static s8 dc_gpio  = KMTERM_HIZ;
static s8 rst_gpio = KMTERM_HIZ;

// Pin number
static int dc_gpio_pin  = KMTERM_PIN_UNK;
static int rst_gpio_pin = KMTERM_PIN_UNK;

// Kernel struct
static struct spi_device * spidev = 0;

// Bufferization
static int buffer_idx = 0;



/*
 *
 * Static functions-helpers
 *
 */

// GPIO initialization as a part of SPI initialization
static int  gpio_init(void);
static void gpio_exit(void);

int kmterm_spi_init(void)
{
    int err;
    struct spi_master * master;
    struct spi_board_info board_info = {
        //.max_speed_hz = 1000000,
        .bus_num      = kmterm_config_dt.spi_busnum,
        .chip_select  = kmterm_config_dt.spi_cs,
        .mode         = kmterm_config_dt.spi_mode
    };
    board_info.max_speed_hz = kmterm_driver_get_spi_speed();
    strncpy(board_info.modalias, kmterm_config_dt.title, SPI_NAME_SIZE);
    buffer = kmalloc(PAGE_SIZE, GFP_KERNEL);

    master = spi_busnum_to_master(board_info.bus_num);
    if ( !master ) {
        KMTERM_ERR("spi_busnum_to_master failed\n");
        return -ENODEV;
    }
    spidev = spi_new_device(master, &board_info);
    if ( !spidev ) {
        KMTERM_ERR("spi_new_device failed\n");
        return -ENODEV;
    }
    spidev->bits_per_word = 8;

    if ( (err = spi_setup(spidev)) > 0 ) {
        KMTERM_ERR("spi_setup failed\n");
        return -ENODEV;
    }
    if ( (err = gpio_init()) ) {
        KMTERM_ERR("GPIO init falied. Exit-code: %d\n", err);
        return err;
    }

    KMTERM_INF("[init] spi\n");
    return 0;
}

void kmterm_spi_exit(void)
{
    if ( spidev ) {
        spi_unregister_device(spidev);
        spidev = 0;
    }
    gpio_exit();
    kfree(buffer);
    KMTERM_INF("~[exit] spi\n");
}

static int gpio_init(void)
{
    int err;

    if ( kmterm_config_dt.gpio_dc >= 0 &&
         (err = gpio_request_one(kmterm_config_dt.gpio_dc, GPIOF_OUT_INIT_HIGH, "kmterm_dc_gpio")) )
    {
        KMTERM_ERR("GPIO %d request failed. Exit-code: %d.\n", dc_gpio_pin, err);
        return -err;
    }
    dc_gpio_pin = kmterm_config_dt.gpio_dc;

    if ( kmterm_config_dt.gpio_rst >= 0 &&
         (err = gpio_request_one(kmterm_config_dt.gpio_rst, GPIOF_OUT_INIT_HIGH, "kmterm_rst_gpio")) )
    {
        KMTERM_ERR("GPIO %d request failed. Exit-code: %d.\n", rst_gpio_pin, err);
        return -err;
    }
    rst_gpio_pin = kmterm_config_dt.gpio_rst;

    KMTERM_INF("[init] gpio\n");
    return 0;
}

static void gpio_exit(void)
{
    if ( dc_gpio_pin > 0 ) {
        gpio_free(dc_gpio_pin);
        dc_gpio_pin = -1;
    }
    if ( rst_gpio_pin > 0 ) {
        gpio_free(rst_gpio_pin);
        rst_gpio_pin = -1;
    }
    KMTERM_INF("~[exit] gpio\n");
}


void kmterm_gpio_rst_set(s8 _val)
{
    if ( rst_gpio != _val &&
         _val != KMTERM_HIZ &&
         rst_gpio_pin != KMTERM_PIN_UNK )
    {
        gpio_set_value(rst_gpio_pin, _val);
        rst_gpio = _val;
    }
}

void kmterm_gpio_dc_set(s8 _val)
{
    if ( dc_gpio != _val &&
         _val != KMTERM_HIZ &&
         dc_gpio_pin != KMTERM_PIN_UNK )
    {
        gpio_set_value(dc_gpio_pin, _val);
        dc_gpio = _val;
    }
}


int kmterm_spi_append(const void * _data, size_t _count)
{
    int err;
    size_t idx = 0, to_copy = 0;
    while ( _count > idx ) {
        to_copy = (PAGE_SIZE - buffer_idx) > (_count - idx)
                ? (_count - idx) : (PAGE_SIZE - buffer_idx);
        memcpy(buffer + buffer_idx, _data + idx, to_copy);
        buffer_idx += to_copy;
        idx += to_copy;
        if ( buffer_idx == PAGE_SIZE && (err = kmterm_spi_flush()) )
            return err;
    }
    return 0;
}
inline int kmterm_spi_flush(void)
{
    int err;
    err = spi_write(spidev, buffer, buffer_idx);
    buffer_idx = 0;
    return err;
}


inline int kmterm_spi_data(const void * _data, int _count)
{
    return spi_write(spidev, _data, _count);
}



static inline int kmterm_send_cmd88(u8 _hi, u8 _lo)
{
    int err;
    kmterm_gpio_dc_set(KMTERM_LOW);
    err = spi_write(spidev, (void *)&_hi, 1);
    err = spi_write(spidev, (void *)&_lo, 1);
    //err = kmterm_spi_flush();
    return err;
}
static inline int kmterm_send_val88(u8 _hi, u8 _lo)
{
    int err;
    kmterm_gpio_dc_set(KMTERM_HIGH);
    err = spi_write(spidev, (void *)&_hi, 1);
    err = spi_write(spidev, (void *)&_lo, 1);
    //err = kmterm_spi_flush();
    return err;
}


inline int kmterm_spi_c16w16(u16 _com, u16 _val)
{
    int err;
    if ( (err = kmterm_send_cmd88(_com >> 8, _com & 0xFF)) )
        return err;
    if ( (err = kmterm_send_val88(_val >> 8, _val & 0xFF)) )
        return err;
    return 0;
}

inline int kmterm_spi_c16(u16 _com)
{
    int err;
    if ( (err = kmterm_send_cmd88(_com >> 8, _com & 0xFF)) )
        return err;
    return 0;
}

inline int kmterm_spi_w16(u16 _com)
{
    int err;
    if ( (err = kmterm_send_val88(_com >> 8, _com & 0xFF)) )
        return err;
    return 0;
}

