#ifndef KMTERM_CDEV_H
#define KMTERM_CDEV_H

#include <linux/fs.h>       // file_operations
#include <linux/device.h>   // (class|device)_(create|destroy)
#include <linux/cdev.h>     // cdev_(add|del|init)
#include <asm/uaccess.h>    // put_user


#include "config.h"


static uint8_t kmterm_cdev_created = 0;
static int kmterm_cdev_major = -1;
static struct class * kmterm_cdev_class = NULL;
static struct cdev kmterm_cdev;



static int kmterm_dev_open_count = 0;
//static uint16_t kmterm_buffer[ kmterm_VSIZE ][ kmterm_HSIZE/8 ];
//static int kmterm_hindex = 0;
//static int kmterm_vindex = 0;

static loff_t  kmterm_device_llseek (struct file *, loff_t, int);
static int     kmterm_device_open   (struct inode *, struct file *);
static int     kmterm_device_release(struct inode *, struct file *);
static ssize_t kmterm_device_read   (struct file *, char *, size_t, loff_t *);
static ssize_t kmterm_device_write  (struct file *, const char *, size_t, loff_t *);

static int  kmterm_chardev_create(void);
static void kmterm_chardev_close (void);


static struct file_operations kmterm_file_ops = {
    .llseek  = kmterm_device_llseek,
    .read    = kmterm_device_read,
    .write   = kmterm_device_write,
    .open    = kmterm_device_open,
    .release = kmterm_device_release
};


static loff_t kmterm_device_llseek (struct file * _file, loff_t _lseek, int _i) {
    /*kmterm_vindex = _lseek/128;
    kmterm_hindex = (_lseek%128)/(128/8);
    return kmterm_vindex + kmterm_hindex*(128/8);*/
    return _lseek;
}

static char buf[(128/8*4 + 1) + 3];
static ssize_t kmterm_device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
    char * ptr = buf;

    /*for ( int i = 0; i < kmterm_VSIZE; ++ i ) {
        for ( int j = 0; j < kmterm_HSIZE/8; ++ j )
           ptr += sprintf(ptr, "%02hhx ", kmterm_buffer[i][j]);
        printk("%s\n", buf);
        ptr = buf;
    }*/
    return -EINVAL;
}
//#include "kmterm_interface.h"
static ssize_t kmterm_device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) {
    //the_thing();
    return len;
    /*uint8_t symb;
    const char * ptr = buffer;
    int consumed, res, num = 0, fd = -1;
    unsigned char oBuf[32], iBuf[32];



    while ( (res = sscanf(ptr, "%hhx%n", &symb, &consumed)) == 1 ) {
        oBuf[num] = symb;
        num ++;

        kmterm_buffer[kmterm_vindex][kmterm_hindex] = symb;
        kmterm_hindex ++ ;
        if ( kmterm_hindex >= kmterm_HSIZE/8 ) {
            kmterm_hindex = 0;
            kmterm_vindex ++;
            if ( kmterm_vindex >= kmterm_VSIZE )
                kmterm_vindex = 0;
        }

        ptr += consumed;
    }*/

    //fd = open("/dev/spidev0.0", O_RDWR);
    //spi_rw(fd, num, oBuf, 32, iBuf);

    return len;
}

/* Called when a process opens our device */
static int kmterm_device_open(struct inode *inode, struct file *file) {
    printk("km: kmterm_device_open\n");
    if ( kmterm_dev_open_count )
        return -EBUSY;
    kmterm_dev_open_count++;
    return 0;
}

static int kmterm_device_release(struct inode *inode, struct file *file) {
    printk("km: kmterm_device_release\n");
    kmterm_dev_open_count--;
    return 0;
}



/*
 *
 *
 * DRIVER INIT
 *
 *
 */



// Character device
int kmterm_chardev_create(void)
{
    int err;

    if ( (err = alloc_chrdev_region(&kmterm_cdev_major, 0, 1, kmterm_config_dt.title)) < 0 ) {
        printk("kmterm_chardev_create: alloc_chrdev_region failed. Exit-code: %d.\n", err);
        kmterm_chardev_close();
        return err;
    }
    if ( (kmterm_cdev_class = class_create(THIS_MODULE, kmterm_config_dt.title)) == NULL ) {
        printk("kmterm_chardev_create: kmterm_cdev_class failed.\n");
        kmterm_chardev_close();
        return -1;
    }
    // returns struct device *
    if ( (device_create(kmterm_cdev_class, NULL, kmterm_cdev_major, NULL, kmterm_config_dt.title)) == NULL ) {
        printk("kmterm_chardev_create: device_create failed.\n");
        kmterm_chardev_close();
        return -1;
    }
    kmterm_cdev_created = 1;
    cdev_init(&kmterm_cdev, &kmterm_file_ops);
    if ( (err = cdev_add(&kmterm_cdev, kmterm_cdev_major, 1)) < 0 ) {
        printk("kmterm_chardev_create: cdev_add failed. Exit-code: %d.\n", err);
        kmterm_chardev_close();
        return err;
    }

    printk("[init] cdev\n");
    return 0;
}
void kmterm_chardev_close(void)
{
    if ( kmterm_cdev_created ) {
        device_destroy(kmterm_cdev_class, kmterm_cdev_major);
        cdev_del(&kmterm_cdev);
        kmterm_cdev_created = 0;
    }
    if ( kmterm_cdev_class ) {
        class_destroy(kmterm_cdev_class);
        kmterm_cdev_class = NULL;
    }
    if ( kmterm_cdev_major != -1 ) {
        unregister_chrdev_region(kmterm_cdev_major, 1);
        kmterm_cdev_major = -1;
    }

    printk("~[exit] cdev\n");
}


#endif // KMTERM_CDEV_H
