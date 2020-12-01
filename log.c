#include "log.h"

#include <linux/module.h>
#include <linux/mutex.h>

static struct mutex log_lock;

static int kmterm_log_level = KMTERM_LOG_MAXIMUM;
static char kmterm_log_buffer[PAGE_SIZE];

int kmterm_log_init(void)
{
    mutex_init(&log_lock);
    return 0;
}

void kmterm_log_setLevel(int _level) {
    if ( _level < KMTERM_LOG_MINIMUM ||
         _level > KMTERM_LOG_MAXIMUM )
    {
        printk("Log level must be between %d and %d.\n",
               KMTERM_LOG_MINIMUM, KMTERM_LOG_MAXIMUM);
        return;
    }
    kmterm_log_level = _level;
    printk("Log level has been set to %d.\n", kmterm_log_level);
    return;
}

void __kmterm_log(const char * _file, int _line, const char * _function,
                   int _level, char * _fmt, ...)
{
    int occupied = 0;
    va_list ap;

    if ( _level > kmterm_log_level )
        return;
    mutex_lock(&log_lock);

    switch ( _level ) {
    case KMTERM_LOG_ERROR:
        occupied = snprintf(kmterm_log_buffer, PAGE_SIZE, KMTERM_LOG_PREFIX "[ERR] ");
        break;
    case KMTERM_LOG_WARNING:
        occupied = snprintf(kmterm_log_buffer, PAGE_SIZE, KMTERM_LOG_PREFIX "[WRN] ");
        break;
    case KMTERM_LOG_INFO:
        occupied = snprintf(kmterm_log_buffer, PAGE_SIZE, KMTERM_LOG_PREFIX "[INF] ");
        break;
    case KMTERM_LOG_DEBUG:
        occupied = snprintf(kmterm_log_buffer, PAGE_SIZE, KMTERM_LOG_PREFIX "[DBG] ");
        break;
    }
    /*occupied += snprintf(kmterm_log_buffer + occupied, PAGE_SIZE - occupied, "%s::%s (%d) ",
                         _file, _function, _line );*/

    va_start (ap, _fmt);
        occupied += vsnprintf(kmterm_log_buffer + occupied, PAGE_SIZE - occupied, _fmt, ap);
    va_end (ap);


    switch ( _level ) {
    case KMTERM_LOG_ERROR:
        printk(KERN_ERR     "%s", kmterm_log_buffer);
        break;
    case KMTERM_LOG_WARNING:
        printk(KERN_WARNING "%s", kmterm_log_buffer);
        break;
    case KMTERM_LOG_INFO:
        printk(KERN_INFO    "%s", kmterm_log_buffer);
        break;
    case KMTERM_LOG_DEBUG:
        printk(KERN_DEBUG   "%s", kmterm_log_buffer);
        break;
    }
    mutex_unlock(&log_lock);


}

void __kmterm_msg(int _level, char * _fmt, ...)
{

    int occupied = 0;
    va_list ap;

    if ( _level > kmterm_log_level )
        return;
    mutex_lock(&log_lock);

    switch ( _level ) {
    case KMTERM_LOG_ERROR:
        occupied = snprintf(kmterm_log_buffer, PAGE_SIZE, KMTERM_LOG_PREFIX "[ERR] ");
        break;
    case KMTERM_LOG_WARNING:
        occupied = snprintf(kmterm_log_buffer, PAGE_SIZE, KMTERM_LOG_PREFIX "[WRN] ");
        break;
    case KMTERM_LOG_INFO:
        occupied = snprintf(kmterm_log_buffer, PAGE_SIZE, KMTERM_LOG_PREFIX "[INF] ");
        break;
    case KMTERM_LOG_DEBUG:
        occupied = snprintf(kmterm_log_buffer, PAGE_SIZE, KMTERM_LOG_PREFIX "[DBG] ");
        break;
    }

    va_start (ap, _fmt);
        occupied += vsnprintf(kmterm_log_buffer + occupied, PAGE_SIZE - occupied, _fmt, ap);
    va_end (ap);


    switch ( _level ) {
    case KMTERM_LOG_ERROR:
        printk(KERN_ERR     "%s", kmterm_log_buffer);
        break;
    case KMTERM_LOG_WARNING:
        printk(KERN_WARNING "%s", kmterm_log_buffer);
        break;
    case KMTERM_LOG_INFO:
        printk(KERN_INFO    "%s", kmterm_log_buffer);
        break;
    case KMTERM_LOG_DEBUG:
        printk(KERN_DEBUG   "%s", kmterm_log_buffer);
        break;
    }
    mutex_unlock(&log_lock);

}