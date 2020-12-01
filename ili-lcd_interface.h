#ifndef ST7735_INTERFACE_H
#define ST7735_INTERFACE_H

#include "spi.h"


int st7735_read_config(int _fd);


void manage_ms5611(int _fd);

#endif // ST7735_INTERFACE_H

#if 0
/////////////////////////////////////////////////////////////////

#define TFT_LED_HEIGTH      128
#define TFT_LED_WIDTH       128
#define TFT_LED_COLOR_DEPTH 2 // bytes per pixel
#define TFT_LED_SIZE        (TFT_LED_HEIGTH * TFT_LED_WIDTH)
#define TFT_LED_DATA_SIZE   (TFT_LED_SIZE   * TFT_LED_COLOR_DEPTH)


#define kmterm_MINUMUM_SLEEP_CMD_MS 100

#define kmterm_DISPLAY_UPDATE_MS 5000


#define kmterm_CMD_SW_RESET 0x01
#define kmterm_CMD_DISPLAY_OFF 0x28
#define kmterm_CMD_DISPLAY_ON 0x29
#define kmterm_CMD_WRITE_START 0x2C
#define kmterm_CMD_PIXEL_FORMAT 0x3A
#define kmterm_CMD_NORMAL_MODE 0x13
#define kmterm_CMD_EXIT_IDLE 0x38
#define CMD_NOP     	0x00//Non operation
#define CMD_SWRESET 	0x01//Soft Reset
#define CMD_SLPIN   	0x10//Sleep ON
#define CMD_SLPOUT  	0x11//Sleep OFF
#define CMD_PTLON   	0x12//Partial Mode ON
#define CMD_NORML   	0x13//Normal Display ON
#define CMD_DINVOF  	0x20//Display Inversion OFF
#define CMD_DINVON   	0x21//Display Inversion ON
#define CMD_GAMMASET 	0x26//Gamma Set (0x01[1],0x02[2],0x04[3],0x08[4])
#define CMD_DISPOFF 	0x28//Display OFF
#define CMD_DISPON  	0x29//Display ON
#define CMD_IDLEON  	0x39//Idle Mode ON
#define CMD_IDLEOF  	0x38//Idle Mode OFF
#define CMD_CLMADRS   	0x2A//Column Address Set
#define CMD_PGEADRS   	0x2B//Page Address Set

#define CMD_RAMWR   	0x2C//Memory Write
#define CMD_RAMRD   	0x2E//Memory Read
#define CMD_CLRSPACE   	0x2D//Color Space : 4K/65K/262K
#define CMD_PARTAREA	0x30//Partial Area
#define CMD_VSCLLDEF	0x33//Vertical Scroll Definition
#define CMD_TEFXLON		0x35//Tearing Effect Line ON
#define CMD_TEFXLOF		0x34//Tearing Effect Line OFF
#define CMD_MADCTL  	0x36//Memory Access Control
#define CMD_VSSTADRS	0x37//Vertical Scrolling Start address
#define CMD_PIXFMT  	0x3A//Interface Pixel Format
#define CMD_FRMCTR1 	0xB1//Frame Rate Control (In normal mode/Full colors)
#define CMD_FRMCTR2 	0xB2//Frame Rate Control(In Idle mode/8-colors)
#define CMD_FRMCTR3 	0xB3//Frame Rate Control(In Partial mode/full colors)
#define CMD_DINVCTR		0xB4//Display Inversion Control
#define CMD_RGBBLK		0xB5//RGB Interface Blanking Porch setting
#define CMD_DFUNCTR 	0xB6//Display Fuction set 5
#define CMD_SDRVDIR 	0xB7//Source Driver Direction Control
#define CMD_GDRVDIR 	0xB8//Gate Driver Direction Control

#define CMD_PWCTR1  	0xC0//Power_Control1
#define CMD_PWCTR2  	0xC1//Power_Control2
#define CMD_PWCTR3  	0xC2//Power_Control3
#define CMD_PWCTR4  	0xC3//Power_Control4
#define CMD_PWCTR5  	0xC4//Power_Control5
#define CMD_VCOMCTR1  	0xC5//VCOM_Control 1
#define CMD_VCOMCTR2  	0xC6//VCOM_Control 2
#define CMD_VCOMOFFS  	0xC7//VCOM Offset Control
#define CMD_PGAMMAC		0xE0//Positive Gamma Correction Setting
#define CMD_NGAMMAC		0xE1//Negative Gamma Correction Setting
#define CMD_GAMRSEL		0xF2//GAM_R_SEL




static const struct of_device_id kmterm_of_device_id_match[] = {
    { .compatible = "spi_bcm2835", },
    { .compatible = "spidev", },
    { },
};
MODULE_DEVICE_TABLE(of, kmterm_of_device_id_match);

static const struct spi_device_id kmterm_spi_id[] = {
    { "kmterm-spi", 0 },
    { }
};
MODULE_DEVICE_TABLE(spi, kmterm_spi_id);


static struct spi_driver kmterm_spi_driver = {
    .driver = {
        .name  = "kmterm-spi",
        .owner = THIS_MODULE,
        .of_match_table = kmterm_of_device_id_match
    },
    .id_table = kmterm_spi_id,
    .probe    = kmterm_spi_probe,
    .remove   = kmterm_spi_remove,
};
#endif

#if 0
    cmd = 0x0C;
    gpio_set_value(kmterm_dc_gpio, 0);
    err = spi_write(kmterm_spidev, (void *)&cmd, 1);
    if ( err ) {
        printk("spi_write FAILED\n");
        return 0;
    }
    gpio_set_value(kmterm_dc_gpio, 1);
    err = spi_read(kmterm_spidev, (void *)res, 6);
    if ( err ) {
        printk("spi_write FAILED\n");
        return 0;
    }
    printk("RES: %02x %02x %02x %02x %02x %02x\n", res[0], res[1], res[2], res[3], res[4], res[5]);

    cmd = 0x0B;
    gpio_set_value(kmterm_dc_gpio, 0);
    err = spi_write(kmterm_spidev, (void *)&cmd, 1);
    if ( err ) {
        printk("spi_write FAILED\n");
        return 0;
    }
    gpio_set_value(kmterm_dc_gpio, 1);
    err = spi_read(kmterm_spidev, (void *)res, 6);
    if ( err ) {
        printk("spi_write FAILED\n");
        return 0;
    }
    printk("RES: %02x %02x %02x %02x %02x %02x\n", res[0], res[1], res[2], res[3], res[4], res[5]);


    cmd = 0x0A;
    gpio_set_value(kmterm_dc_gpio, 0);
    err = spi_write(kmterm_spidev, (void *)&cmd, 1);
    if ( err ) {
        printk("spi_write FAILED\n");
        return 0;
    }
    gpio_set_value(kmterm_dc_gpio, 1);
    err = spi_read(kmterm_spidev, (void *)res, 6);
    if ( err ) {
        printk("spi_write FAILED\n");
        return 0;
    }
    printk("RES: %02x %02x %02x %02x %02x %02x\n", res[0], res[1], res[2], res[3], res[4], res[5]);


    cmd = 0x09;
    gpio_set_value(kmterm_dc_gpio, 0);
    err = spi_write(kmterm_spidev, (void *)&cmd, 1);
    if ( err ) {
        printk("spi_write FAILED\n");
        return 0;
    }
    gpio_set_value(kmterm_dc_gpio, 1);
    err = spi_read(kmterm_spidev, (void *)res, 6);
    if ( err ) {
        printk("spi_write FAILED\n");
        return 0;
    }
    printk("RES: %02x %02x %02x %02x %02x %02x\n", res[0], res[1], res[2], res[3], res[4], res[5]);

    ////////////////////////////////////////
    cmd = 0x04;
    gpio_set_value(kmterm_dc_gpio, 0);
    err = spi_write(kmterm_spidev, (void *)&cmd, 1);
    if ( err ) {
        printk("spi_write FAILED\n");
        return 0;
    }
    gpio_set_value(kmterm_dc_gpio, 1);
    err = spi_read(kmterm_spidev, (void *)res, 6);
    if ( err ) {
        printk("spi_write FAILED\n");
        return 0;
    }
    printk("RES: %02x %02x %02x %02x %02x %02x\n", res[0], res[1], res[2], res[3], res[4], res[5]);
    ////////////////////////////////////////

    uint16_t watchman_image[256];

    printk("0x%04hx ---=== THE THING ===---\n", color);

    for ( int i = 0; i < 256; ++ i ) {
        watchman_image[i] = color;
    }

    color += 0x1024;
    gpio_set_value(kmterm_dc_gpio, 1);
    err = kmterm_send_cmd(kmterm_CMD_WRITE_START);
    if(err < 0)
        return err;

    for(int i = 0; i < TFT_LED_DATA_SIZE; i += sizeof(uint16_t)) {
        err = kmterm_send_data((void *)watchman_image, sizeof(uint16_t));
        if(err < 0) {
            printk("spi_write error \n");
            break;
        }
    }




    /*static int the_thing(void) {

        int err;
        u8 cmd;
        u8 res[32];

        uint16_t watchman_image[256];


        printk("0x%04hx ---=== THE THING ===---\n", color);

        for ( int i = 0; i < 256; ++ i ) {
            watchman_image[i] = color;
        }

        color += 0x1024;


        kmterm_send_command16(0x00, 0x22);


        gpio_set_value(kmterm_dc_gpio, 1);

        for (int i = 0; i < TFT_LED_DATA_SIZE; i += sizeof(uint16_t)) {
            if ( (err = kmterm_send_data((void *)watchman_image, sizeof(uint16_t)) < 0) ) {
                printk("spi_write error \n");
                break;
            }
        }
        gpio_set_value(kmterm_dc_gpio, 0);

        return 0;
    }*/

#endif