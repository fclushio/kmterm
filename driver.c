#include "driver.h"
#include "log.h"


enum kmterm_driver_type
{
    kmterm_driver_invalid,
    kmterm_driver_ili9225
};

static enum kmterm_driver_type driver_type = kmterm_driver_invalid;

static int kmterm_driver_check_config(void)
{
    struct kmterm_driver_config * config = 0;
    switch ( driver_type ) {
    case kmterm_driver_invalid:
        KMTERM_ERR("kmterm_driver_check_config: kmterm_driver_invalid\n");
        return -EINVAL;
    case kmterm_driver_ili9225:
        config = &ili9225_driver_config;
        break;
    }

    if ( config->gpio_required & KMTERM_DRIVER_GPIO_RST &&
         kmterm_config_dt.gpio_rst < 0)
    {
        KMTERM_ERR("gpio_rst is required to be set for this driver.\n");
        return -EINVAL;
    }
    if ( config->gpio_required & KMTERM_DRIVER_GPIO_DC &&
         kmterm_config_dt.gpio_dc < 0)
    {
        KMTERM_ERR("dpio_dc is required to be set for this driver.\n");
        return -EINVAL;
    }
    if ( config->maximum_height < kmterm_config_dt.display_height ||
         config->minimum_height > kmterm_config_dt.display_height )
    {
        KMTERM_ERR("display_height: invald value");
        return -EINVAL;
    }
    if ( config->maximum_width < kmterm_config_dt.display_width ||
         config->maximum_width > kmterm_config_dt.display_width )
    {
        KMTERM_ERR("display_width: invald value");
        return -EINVAL;
    }
    return 0;
}
u32 kmterm_driver_get_spi_speed(void)
{
    u32 spi_maxspeed = 0;
    switch ( driver_type ) {
    case kmterm_driver_invalid:
        KMTERM_ERR("kmterm_driver_get_spi_speed: kmterm_driver_invalid\n");
        return 0;
    case kmterm_driver_ili9225:
        spi_maxspeed = ili9225_driver_config.spi_maxspeed;
        break;
    }

    return kmterm_config_dt.spi_maxspeed > spi_maxspeed
            ? spi_maxspeed : kmterm_config_dt.spi_maxspeed;
}

int kmterm_driver_init(void)
{
    int err = 0;
    if ( !strcmp(kmterm_config_dt.driver, ILI9225_DRIVER_NAME) ) {
        driver_type = kmterm_driver_ili9225;
        if ( (err = kmterm_driver_check_config()) ) {
            KMTERM_ERR("Bad DT config\n");
            return err;
        }
        KMTERM_INF("[init] ili9225 driver\n");
        return 0;
    }
    return -EINVAL;
}


int kmterm_device_init(void)
{
    int err = 0;
    switch ( driver_type ) {
    case kmterm_driver_invalid:
        KMTERM_ERR("kmterm_device_init: kmterm_driver_invalid\n");
        return -EINVAL;
    case kmterm_driver_ili9225:
        if ( (err = ili9225_init()) ) {
            KMTERM_ERR("ili9225_init failed. Exit-code: %d.\n", err);
            return err;
        }
        KMTERM_INF("[init] ili9225 device\n");
        return 0;
    }
    return -EINVAL;
}

int kmterm_draw_character(u16 _h, u16 _v, struct character _ch)
{
    switch ( driver_type ) {
    case kmterm_driver_invalid:
        KMTERM_ERR("kmterm_draw_character: kmterm_driver_invalid\n");
        return -EINVAL;
    case kmterm_driver_ili9225:
        return ili9225_draw_character(_h,_v, _ch);
    }
    return -EINVAL;
}

int kmterm_draw_zone(int _h0, int _h1, int _v0, int _v1, struct character *_data)
{
    switch ( driver_type ) {
    case kmterm_driver_invalid:
        KMTERM_ERR("kmterm_draw_character: kmterm_driver_invalid\n");
        return -EINVAL;
    case kmterm_driver_ili9225:
        return ili9225_draw_zone(_h0, _h1, _v0, _v1, _data);
    }
    return -EINVAL;
}

int kmterm_clear_screen(void)
{
    switch ( driver_type ) {
    case kmterm_driver_invalid:
        KMTERM_ERR("kmterm_clear_screen: kmterm_driver_invalid\n");
        return -EINVAL;
    case kmterm_driver_ili9225:
        return ili9225_clear_screen();
    }
    return -EINVAL;
}
