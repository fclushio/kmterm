#include "io.h"

#include "font/font.h"
#include "terminal.h"
#include "config.h"
#include "tty.h"
#include "log.h"

#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/tty.h>


unsigned char char_queue[PAGE_SIZE];
static struct mutex char_queue_lock; // Lock for accessing s_CharQueue
int append_pos   = 0;   // End of the queue position
int transmit_pos = 0; // Start of the queue position


static struct wait_queue_head task_queue;
static struct task_struct * task;
static struct semaphore task_join_sem;
u8 fSleeping = 0; // Being set if the thread sleeps, so it could be woken up
u8 fWorking  = 0; // Working flag, wait for safe transmitting end



int terminal_worker(void * _data);

static struct kmterm_tty user_tty;
static struct kmterm_tty shell_tty;


static int on_user_write (struct tty_struct * _tty, const unsigned char * _buffer, int _count);
static int on_shell_write(struct tty_struct * _tty, const unsigned char * _buffer, int _count);

static int on_write_room     (struct tty_struct * _tty);
static int on_chars_in_buffer(struct tty_struct * _tty);

const static struct kmterm_tty_ops user_ops = {
    .write           = on_user_write,
    .write_room      = on_write_room,
    .chars_in_buffer = on_chars_in_buffer
};
const static struct kmterm_tty_ops shell_ops = {
    .write           = on_shell_write,
    .write_room      = on_write_room,
    .chars_in_buffer = on_chars_in_buffer
};

int kmterm_io_init(void)
{
    sema_init(&task_join_sem, 1);
    mutex_init(&char_queue_lock);
    init_waitqueue_head(&task_queue);
    fWorking = 1;
    task = kthread_run(terminal_worker, 0, "kmterm_terminal_worker");

    kmterm_tty_init("kmterm",       &user_ops,  &user_tty);
    kmterm_tty_init("shell_kmterm", &shell_ops, &shell_tty);


    KMTERM_INF("[init] io\n");
    return 0;
}

void kmterm_io_exit(void)
{
    // Clear the queue indexes, wake up thread
    mutex_lock(&char_queue_lock);
    {
        fWorking = 0;
        append_pos = 0;
        transmit_pos = 0;
        if ( fSleeping )
            wake_up(&task_queue);
    }
    mutex_unlock(&char_queue_lock);
    down(&task_join_sem); // join thread

    kmterm_tty_exit(&user_tty);
    kmterm_tty_exit(&shell_tty);

    KMTERM_INF("~[exit] io\n");
}

void kmterm_io_resize(void)
{
    struct winsize ws = {
        .ws_row    =
                kmterm_config_sysfs.orientation ?
                    kmterm_config_dt.display_width /
                    kmterm_font_size[ kmterm_config_sysfs.font ].h :
                    kmterm_config_dt.display_height /
                    kmterm_font_size[ kmterm_config_sysfs.font ].v,
        .ws_col    =
                kmterm_config_sysfs.orientation ?
                    kmterm_config_dt.display_height /
                    kmterm_font_size[ kmterm_config_sysfs.font ].v :
                    kmterm_config_dt.display_width /
                    kmterm_font_size[ kmterm_config_sysfs.font ].h,
        .ws_xpixel = kmterm_font_size[ kmterm_config_sysfs.font ].h,
        .ws_ypixel = kmterm_font_size[ kmterm_config_sysfs.font ].v
    };
    if ( user_tty.port.count )
        tty_do_resize(user_tty.port.tty, &ws);
    if ( shell_tty.port.count )
        tty_do_resize(shell_tty.port.tty, &ws);
}

static inline void kmterm_terminal_handle_char(unsigned char _ch)
{
    char_queue[append_pos] = _ch;
    append_pos++;
    if ( append_pos == transmit_pos )
        KMTERM_ERR("CRITICAL: append has reached transmit\n");
    if ( append_pos == PAGE_SIZE )
        append_pos = 0;

    if ( fSleeping )
        wake_up(&task_queue);
}

static inline void notify_shell(void)
{
    struct ktermios kterm;
    struct tty_struct * tty;
    tty = shell_tty.driver->ttys[0];
    if ( tty ) {
        down_read(&tty->termios_rwsem);
        kterm = tty->termios;
        up_read(&tty->termios_rwsem);
        tty_set_termios(tty, &kterm);
    }
}


int terminal_worker(void * _data)
{
    (void)_data;
    down(&task_join_sem);

    while ( fWorking )
    {
        mutex_lock(&char_queue_lock);

        if ( transmit_pos == append_pos ) {
            fSleeping = 1;
            mutex_unlock(&char_queue_lock);
            wait_event_interruptible(task_queue, (!fWorking) || (transmit_pos != append_pos) );
            fSleeping = 0;
        } else {
            kmterm_process_char( char_queue[transmit_pos] );
            transmit_pos++;
            if ( transmit_pos == PAGE_SIZE )
                transmit_pos = 0;
            mutex_unlock(&char_queue_lock);
            if ( transmit_pos == append_pos )
                notify_shell();
        }
    }

    up(&task_join_sem);
    return 0;
}

static int on_user_write(struct tty_struct * _tty, const unsigned char * _buffer, int _count)
{
    int err;

    for ( int i = 0; i < _count; ++ i ) {
        if ( !tty_buffer_request_room(&shell_tty.port, 1) )
            tty_flip_buffer_push(&shell_tty.port);
        if ( (err = tty_insert_flip_char(&shell_tty.port, _buffer[i], TTY_NORMAL)) != 1 )
            KMTERM_WRN("user : to user  tty_insert_flip_char failed %d\n", err);
    }
    tty_flip_buffer_push(&shell_tty.port);

    return _count;
}

static int on_shell_write(struct tty_struct * _tty, const unsigned char * _buffer, int _count)
{
    int room = on_write_room(_tty);
    int max = ( _count > room ) ? room : _count;

    printk("[dump] shell: ");
    for ( int i = 0 ; i < _count; ++ i ) {
        if ( _buffer[i] < 32 )
            printk(KERN_CONT "--==%02x==-- ", _buffer[i]);
        else
            printk(KERN_CONT "%02x ", _buffer[i]);
    }
    printk(KERN_CONT "\n[dump] shell: ");
    for ( int i = 0 ; i < _count; ++ i ) {
        if ( _buffer[i] < 32 )
            printk(KERN_CONT "_");
        else
            printk(KERN_CONT "%c", _buffer[i]);
    }
    printk(KERN_CONT "\n");

    mutex_lock(&char_queue_lock);
    for ( int i = 0; i < max; ++ i )
        kmterm_terminal_handle_char( _buffer[i] );
    mutex_unlock(&char_queue_lock);

    return max;
}


static int on_write_room(struct tty_struct * _tty)
{
    int res;
    mutex_lock(&char_queue_lock);
    res = (transmit_pos > append_pos ) ?
                (append_pos - transmit_pos) :
                (PAGE_SIZE - (append_pos - transmit_pos));
    mutex_unlock(&char_queue_lock);
    return res;
}

static int on_chars_in_buffer(struct tty_struct * _tty)
{
    int res;
    mutex_lock(&char_queue_lock);

    res = (transmit_pos < append_pos ) ?
                (append_pos - transmit_pos) :
                (PAGE_SIZE - (append_pos - transmit_pos));
    mutex_unlock(&char_queue_lock);
    return res % PAGE_SIZE;
}
