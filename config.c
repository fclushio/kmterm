#include <linux/module.h>   // basic kernel module functionality
#include <linux/fs.h>       // file_operations
#include <linux/device.h>   // (class|device)_(create|destroy)
#include <linux/cdev.h>     // cdev_(add|del|init)
#include <asm/uaccess.h>    // put_user
#include <linux/spi/spi.h>
#include <linux/gpio.h>
#include <linux/delay.h>    // msleep

#include <linux/sysfs.h>    // kobject
#include <linux/kobject.h>  // kobject


#include "log.h"
#include "config.h"
#include "font/font.h"

static ssize_t print_help(char * _buf)
{
    return sprintf(_buf, "rtfm!\n");
}


void (*kmterm_font_callback)       (int _font)        = 0;
void (*kmterm_color_callback)      (int _color)       = 0;
void (*kmterm_orientation_callback)(int _orientation) = 0;
static void (*kmterm_log_level_callback)(int _level) = kmterm_log_setLevel;

/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                    DEVICE TREE
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */


struct kmterm_dt_config_t kmterm_config_dt = {
    // Not required to be set via DT
    .title          = "kmterm",
    .spi_maxspeed   = 400000,
    .spi_busnum     = 0,
    .spi_cs         = 0,
    .spi_mode       = 0,
    // Mandatory
    .driver         = "invalid",
    .display_width  = -1,
    .display_height = -1,
    .gpio_dc        = -1,
    .gpio_rst       = -1
};


static int config_dt_init(const char * _dtnode)
{
    int err;
    struct device_node * node;

    node = of_find_node_by_name(NULL, _dtnode);
    if ( !node ) {
        printk("Node %s was not found\n", _dtnode);
        return -ENOENT;
    }
    do {
        if ( (err = of_property_read_string(node, "title",
                                            &kmterm_config_dt.title)) )
        {
            if ( err != -EINVAL ) { // Field could be omitted
                KMTERM_ERR("Failed to read field \'title\'. Exit-code: %d", err);
                break;
            }
        }
        if ( (err = of_property_read_u32(node, "#spi_maxspeed",
                                         &kmterm_config_dt.spi_maxspeed)) )
        {
            if ( err != -EINVAL ) { // Field could be omitted
                KMTERM_ERR("Failed to read field \'spi_maxspeed\'. Exit-code: %d", err);
                break;
            }
        }
        if ( (err = of_property_read_s32(node, "#spi_busnum",
                                         &kmterm_config_dt.spi_busnum)) )
        {
            if ( err != -EINVAL ) { // Field could be omitted
                KMTERM_ERR("Failed to read field \'spi_busnum\'. Exit-code: %d", err);
                break;
            }
        }
        if ( (err = of_property_read_s32(node, "#spi_cs",
                                         &kmterm_config_dt.spi_cs)) )
        {
            if ( err != -EINVAL ) { // Field could be omitted
                KMTERM_ERR("Failed to read field \'spi_cs\'. Exit-code: %d", err);
                break;
            }
        }
        if ( (err = of_property_read_s32(node, "#spi_mode",
                                         &kmterm_config_dt.spi_mode)) )
        {
            if ( err != -EINVAL ) { // Field could be omitted
                KMTERM_ERR("Failed to read field \'spi_mode\'. Exit-code: %d", err);
                break;
            }
        }
        if ( (err = of_property_read_s32(node, "#gpio_dc",
                                         &kmterm_config_dt.gpio_dc)) )
        {
            if ( err != -EINVAL ) { // Field could be omitted
                KMTERM_ERR("Failed to read field \'gpio_dc\'. Exit-code: %d", err);
                break;
            }
        }
        if ( (err = of_property_read_s32(node, "#gpio_rst",
                                         &kmterm_config_dt.gpio_rst)) )
        {
            if ( err != -EINVAL ) { // Field could be omitted
                KMTERM_ERR("Failed to read field \'gpio_rst\'. Exit-code: %d", err);
                break;
            }
        }

        // Mandatory
        if ( (err = of_property_read_string(node, "driver",
                                            &kmterm_config_dt.driver)) )
        {
            KMTERM_ERR("Failed to read field \'driver\'. Exit-code: %d", err);
            break;
        }
        if ( (err = of_property_read_s32(node, "#display_width",
                                         &kmterm_config_dt.display_width)) )
        {
            KMTERM_ERR("Failed to read field \'display_width\'. Exit-code: %d", err);
            break;
        }
        if ( (err = of_property_read_s32(node, "#display_height",
                                         &kmterm_config_dt.display_height)) )
        {
            KMTERM_ERR("Failed to read field \'display_height\'. Exit-code: %d", err);
            break;
        }

        KMTERM_MESG("--== CURRENT CONFIG ==--\n");
        KMTERM_MESG("title:\t\t%s\n",        kmterm_config_dt.title);
        KMTERM_MESG("spi_maxspeed:\t%u\n",   kmterm_config_dt.spi_maxspeed);
        KMTERM_MESG("spi_busnum:\t%d\n",     kmterm_config_dt.spi_busnum);
        KMTERM_MESG("spi_cs:\t\t%d\n",       kmterm_config_dt.spi_cs);
        KMTERM_MESG("spi_mode:\t%d\n",       kmterm_config_dt.spi_mode);

        KMTERM_MESG("driver:\t\t%s\n",       kmterm_config_dt.driver);
        KMTERM_MESG("display_width:\t%d\n",  kmterm_config_dt.display_width);
        KMTERM_MESG("display_height:\t%d\n", kmterm_config_dt.display_height);
        KMTERM_MESG("gpio_dc:\t%d\n",        kmterm_config_dt.gpio_dc);
        KMTERM_MESG("gpio_rst:\t%d\n",       kmterm_config_dt.gpio_rst);

        return 0;
    } while ( 0 );

    //of_free_node(node);
    return err;
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *
 *
 *                      SYSFS
 *
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Setting enums
 */


enum color_enum {
    color_min = 0,
    color_basic = color_min,
    color_max
};
enum orientation_enum {
    orientation_min = 0,
    orientation_horizontal = orientation_min,
    orientation_vertical,
    orientation_max
};
enum log_level_enum {
    log_level_min     = KMTERM_LOG_MINIMUM,
    log_level_no      = KMTERM_LOG_NO,
    log_level_error   = KMTERM_LOG_ERROR,
    log_level_warning = KMTERM_LOG_WARNING,
    log_level_info    = KMTERM_LOG_INFO,
    log_level_debug   = KMTERM_LOG_DEBUG,
    log_level_max
};



struct kmterm_sysfs_config_t kmterm_config_sysfs = {
    .font        = font_basic,
    .color       = color_basic,
    .orientation = orientation_horizontal,
    .log_level   = KMTERM_LOG_MAXIMUM
};

const u16 kmterm_color_table[] = { // B R G (5bit for Green, 6bit for Red, 5bit for Blue), switched to RGB
    [kmterm_color_black  ] = ((0   >> 2) << 5)  |  ((0   >> 3) << 0)  |  ((0   >> 3) << 11),
    [kmterm_color_red    ] = ((85  >> 2) << 5)  |  ((0   >> 3) << 0)  |  ((0   >> 3) << 11), // 0x2f << 5
    [kmterm_color_green  ] = ((0   >> 2) << 5)  |  ((255 >> 3) << 0)  |  ((0   >> 3) << 11), // 0x1f << 0
    [kmterm_color_yellow ] = ((85  >> 2) << 5)  |  ((85  >> 3) << 0)  |  ((0   >> 3) << 11),
    [kmterm_color_blue   ] = ((0   >> 2) << 5)  |  ((0   >> 3) << 0)  |  ((170 >> 3) << 11), // 0x1f << 11
    [kmterm_color_magenta] = ((85  >> 2) << 5)  |  ((0   >> 3) << 0)  |  ((170 >> 3) << 11),
    [kmterm_color_cyan   ] = ((0   >> 2) << 5)  |  ((255 >> 3) << 0)  |  ((255 >> 3) << 11),
    [kmterm_color_white  ] = ((255 >> 2) << 5)  |  ((255 >> 3) << 0)  |  ((255 >> 3) << 11),
    [kmterm_color_bright_black  ] = ((85  >> 2) << 5)  |  ((85  >> 3) << 0)  |  ((85  >> 3) << 11),
    [kmterm_color_bright_red    ] = ((255 >> 2) << 5)  |  ((85  >> 3) << 0)  |  ((85  >> 3) << 11),
    [kmterm_color_bright_green  ] = ((85  >> 2) << 5)  |  ((255 >> 3) << 0)  |  ((85  >> 3) << 11),
    [kmterm_color_bright_yellow ] = ((255 >> 2) << 5)  |  ((255 >> 3) << 0)  |  ((85  >> 3) << 11),
    [kmterm_color_bright_blue   ] = ((85  >> 2) << 5)  |  ((85  >> 3) << 0)  |  ((255 >> 3) << 11),
    [kmterm_color_bright_magenta] = ((255 >> 2) << 5)  |  ((85  >> 3) << 0)  |  ((255 >> 3) << 11),
    [kmterm_color_bright_cyan   ] = ((85  >> 2) << 5)  |  ((255 >> 3) << 0)  |  ((255 >> 3) << 11),
    [kmterm_color_bright_white  ] = ((255 >> 2) << 5)  |  ((255 >> 3) << 0)  |  ((255 >> 3) << 11),
};
/*
 *
const u16 kmterm_color_table[] = { // B R G (5bit for Green, 6bit for Red, 5bit for Blue), switched to RGB
    [kmterm_color_black  ] = ((0   >> 2) << 5)  |  ((0   >> 3) << 0)  |  ((0   >> 3) << 11),
    [kmterm_color_red    ] = ((170 >> 2) << 5)  |  ((0   >> 3) << 0)  |  ((0   >> 3) << 11), // 0x2f << 5
    [kmterm_color_green  ] = ((0   >> 2) << 5)  |  ((170 >> 3) << 0)  |  ((0   >> 3) << 11), // 0x1f << 0
    [kmterm_color_yellow ] = ((170 >> 2) << 5)  |  ((85  >> 3) << 0)  |  ((0   >> 3) << 11),
    [kmterm_color_blue   ] = ((0   >> 2) << 5)  |  ((0   >> 3) << 0)  |  ((170 >> 3) << 11), // 0x1f << 11
    [kmterm_color_magenta] = ((170 >> 2) << 5)  |  ((0   >> 3) << 0)  |  ((170 >> 3) << 11),
    [kmterm_color_cyan   ] = ((0   >> 2) << 5)  |  ((170 >> 3) << 0)  |  ((170 >> 3) << 11),
    [kmterm_color_white  ] = ((170 >> 2) << 5)  |  ((170 >> 3) << 0)  |  ((170 >> 3) << 11),
    [kmterm_color_bright_black  ] = ((85  >> 2) << 5)  |  ((85  >> 3) << 0)  |  ((85  >> 3) << 11),
    [kmterm_color_bright_red    ] = ((255 >> 2) << 5)  |  ((85  >> 3) << 0)  |  ((85  >> 3) << 11),
    [kmterm_color_bright_green  ] = ((85  >> 2) << 5)  |  ((255 >> 3) << 0)  |  ((85  >> 3) << 11),
    [kmterm_color_bright_yellow ] = ((255 >> 2) << 5)  |  ((255 >> 3) << 0)  |  ((85  >> 3) << 11),
    [kmterm_color_bright_blue   ] = ((85  >> 2) << 5)  |  ((85  >> 3) << 0)  |  ((255 >> 3) << 11),
    [kmterm_color_bright_magenta] = ((255 >> 2) << 5)  |  ((85  >> 3) << 0)  |  ((255 >> 3) << 11),
    [kmterm_color_bright_cyan   ] = ((85  >> 2) << 5)  |  ((255 >> 3) << 0)  |  ((255 >> 3) << 11),
    [kmterm_color_bright_white  ] = ((255 >> 2) << 5)  |  ((255 >> 3) << 0)  |  ((255 >> 3) << 11),
};
*/
struct sysfs_param {
    int * value;
    int min;
    int max;
    const char * name;
    const char * const * const description;
    void (**callback)(int _arg);
};



/*
 *  Font param data
 */
static const char * const font_description[] = {
    [font_basic] = "Basic (8x14)",
    0
};
static const struct sysfs_param font_param = {
    .name  = "font",
    .value = &kmterm_config_sysfs.font,
    .min   = font_min,
    .max   = font_max,
    .description = font_description,
    .callback = &kmterm_font_callback
};


/*
 *  Color param data
 */
static const char * const color_description[] = {
    [color_basic] = "Basic",
    0
};
static const struct sysfs_param color_param = {
    .name  = "color",
    .value = &kmterm_config_sysfs.color,
    .min   = color_min,
    .max   = color_max,
    .description = color_description,
    .callback = &kmterm_color_callback
};


/*
 * Log level
 */
static const char * const log_level_description[] = {
    [log_level_no]      = "No logs",
    [log_level_error]   = "Error",
    [log_level_warning] = "Warning",
    [log_level_info]    = "Info",
    [log_level_debug]   = "Debug",
    0
};
static const struct sysfs_param log_level_param = {
    .name  = "log_level",
    .value = &kmterm_config_sysfs.log_level,
    .min   = KMTERM_LOG_MINIMUM,
    .max   = KMTERM_LOG_MAXIMUM + 1,
    .description = log_level_description,
    .callback = &kmterm_log_level_callback
};


/*
 *  Orientation param data
 */
static const char * const orientation_description[] = {
    [orientation_horizontal] = "horizontal",
    [orientation_vertical]   = "vertical",
    0
};
static const struct sysfs_param orientation_param = {
    .name  = "orientation",
    .value = &kmterm_config_sysfs.orientation,
    .min   = orientation_min,
    .max   = orientation_max,
    .description = orientation_description,
    .callback = &kmterm_orientation_callback
};


/*
 * Help param data
 */
static const struct sysfs_param param_help = {
    .name  = "help",
    .min   = 0,
    .max   = -1
};


static const struct sysfs_param * const  sysfs_params[] = {
    &orientation_param,
    &font_param,
    &color_param,
    &log_level_param,
    &param_help,
    0
};



static ssize_t kobj_show (struct kobject * _kobj, struct attribute * _attr,
                          char * _buf);
static ssize_t kobj_store(struct kobject * _kobj, struct attribute * _attr,
                          const char * _buf, size_t _count);

static struct attribute ** sysfs_attributes = 0;

static struct sysfs_ops sysfs_ops = {
    .show  = kobj_show,
    .store = kobj_store
};

static struct kobj_type kobj_type = {
    .sysfs_ops     = &sysfs_ops
};


static struct kobject * sysfs_kobj = 0;


static int config_sysfs_init(void)
{
    int err = 0;
    int i = 0;
    struct attribute * cur_attr;

    if ( (sysfs_kobj = kzalloc(sizeof(*sysfs_kobj), GFP_KERNEL)) == 0 ) {
        KMTERM_ERR("kzalloc failed.\n");
        return -ENOMEM;
    }

    // Allocation of attributes
    sysfs_attributes = kzalloc( sizeof(sysfs_params), GFP_KERNEL );
    while ( sysfs_params[i] ) {
        cur_attr = kzalloc( sizeof(struct attribute), GFP_KERNEL );
        cur_attr->name = sysfs_params[i]->name;
        cur_attr->mode = 0660;
        sysfs_attributes[i] = cur_attr;
        i++;
    }
    kobj_type.default_attrs = sysfs_attributes;

    kobject_init(sysfs_kobj, &kobj_type);
    if ( (err = kobject_add(sysfs_kobj, 0, "%s", kmterm_config_dt.title)) ) {
        KMTERM_ERR("kobject_add failed. Exit-code: %d.\n", err);
        return err;
    }

    return 0;
}

static void sysfs_exit(void)
{
    int i;
    if ( sysfs_kobj ) {
        kobject_put(sysfs_kobj);
        kfree(sysfs_kobj);
        sysfs_kobj = 0;
    }
    // freeing allocated memory
    i = 0;
    while ( sysfs_params[i] ) {
        kfree(sysfs_attributes[i]);
        i++;
    }
    kfree(sysfs_attributes);
}

static ssize_t kobj_show(struct kobject * _kobj, struct attribute * _attr,
                         char * _buf)
{
    int i = 0;
    if ( strcmp(_attr->name, "help") == 0 )
        return print_help(_buf);

    while ( sysfs_params[i] ) {
        if ( strcmp(_attr->name, sysfs_params[i]->name) == 0 ) {
            return sprintf(_buf, "%d -- %s\n", *sysfs_params[i]->value,
                           sysfs_params[i]->description[ *sysfs_params[i]->value]);
        }
        i++;
    }
    KMTERM_ERR("KObject for \'%s\' not found.\n", _attr->name);
    return sprintf(_buf, "Not found");


}
static ssize_t kobj_store(struct kobject * _kobj, struct attribute * _attr,
                          const char * _buf, size_t _count)
{
    int i = 0, res;
    while ( sysfs_params[i] ) {
        if ( strcmp(_attr->name, sysfs_params[i]->name) == 0 ) {
            if ( sscanf(_buf, "%d", &res) != 1 ) {
                KMTERM_ERR("Bad setting value for %s : %s.\n", _attr->name, _buf);
                return _count;
            }
            if ( res > sysfs_params[i]->max ||
                 res < sysfs_params[i]->min )
            {
                KMTERM_ERR("Range check failed for setting value for \'%s\' : \'%s\'."
                           "Value must be between \'%d\' and \'%d\'.\n", _attr->name, _buf,
                           sysfs_params[i]->min, sysfs_params[i]->max);
                return _count;
            }
            *sysfs_params[i]->value = res;
            KMTERM_DBG("New value for \'%s\' : %d.\n", _attr->name, res);

            // Callbacks
            if ( sysfs_params[i]->callback && *sysfs_params[i]->callback )
                (*sysfs_params[i]->callback)(res);

            return _count;
        }
        i++;
    }
    KMTERM_ERR("Was not able to find attribute \'%s\' in array.\n", _attr->name);
    return _count;
}


int kmterm_config_init(void)
{
    int err;
    if ( (err = config_dt_init(KMTERM_DT_NODE_TITLE)) )
        return err;
    if ( (err = config_sysfs_init()) )
        return err;
    KMTERM_INF("[init] config\n");
    return 0;
}

void kmterm_config_exit(void)
{
    sysfs_exit();
    KMTERM_INF("~[exit] config\n");
}
