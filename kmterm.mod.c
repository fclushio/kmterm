#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x367398b6, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x4e251c31, __VMLINUX_SYMBOL_STR(kobject_put) },
	{ 0x7f9f07e3, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x12da5bb2, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0xff178f6, __VMLINUX_SYMBOL_STR(__aeabi_idivmod) },
	{ 0x665007e4, __VMLINUX_SYMBOL_STR(up_read) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0xcfb4193f, __VMLINUX_SYMBOL_STR(tty_do_resize) },
	{ 0xb6e19285, __VMLINUX_SYMBOL_STR(tty_port_open) },
	{ 0x3ee15874, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
	{ 0x403f9529, __VMLINUX_SYMBOL_STR(gpio_request_one) },
	{ 0x43bef699, __VMLINUX_SYMBOL_STR(tty_register_driver) },
	{ 0xe4ca3b4f, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0x63b8929f, __VMLINUX_SYMBOL_STR(tty_buffer_request_room) },
	{ 0x375d776a, __VMLINUX_SYMBOL_STR(put_tty_driver) },
	{ 0xf4e3cef0, __VMLINUX_SYMBOL_STR(spi_setup) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0xa4ea8452, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0xcbdf8d64, __VMLINUX_SYMBOL_STR(tty_set_operations) },
	{ 0xe2d5255a, __VMLINUX_SYMBOL_STR(strcmp) },
	{ 0xe5bd22c1, __VMLINUX_SYMBOL_STR(down_read) },
	{ 0x93de854a, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0xe707d823, __VMLINUX_SYMBOL_STR(__aeabi_uidiv) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x5f754e5a, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x90ec044, __VMLINUX_SYMBOL_STR(__tty_insert_flip_char) },
	{ 0xd6d20c3b, __VMLINUX_SYMBOL_STR(tty_port_close) },
	{ 0xcc7f381d, __VMLINUX_SYMBOL_STR(spi_busnum_to_master) },
	{ 0x2aee63f4, __VMLINUX_SYMBOL_STR(__mutex_init) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x20c55ae0, __VMLINUX_SYMBOL_STR(sscanf) },
	{ 0x75dffb0d, __VMLINUX_SYMBOL_STR(of_property_read_string) },
	{ 0xa1c76e0a, __VMLINUX_SYMBOL_STR(_cond_resched) },
	{ 0x328a05f1, __VMLINUX_SYMBOL_STR(strncpy) },
	{ 0xc08cae57, __VMLINUX_SYMBOL_STR(spi_new_device) },
	{ 0xc1065dac, __VMLINUX_SYMBOL_STR(tty_port_init) },
	{ 0xc0d4b366, __VMLINUX_SYMBOL_STR(tty_port_destroy) },
	{ 0xfd16e532, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0xf473ffaf, __VMLINUX_SYMBOL_STR(down) },
	{ 0x4810c27a, __VMLINUX_SYMBOL_STR(kobject_add) },
	{ 0xfe487975, __VMLINUX_SYMBOL_STR(init_wait_entry) },
	{ 0x2196324, __VMLINUX_SYMBOL_STR(__aeabi_idiv) },
	{ 0x8e897994, __VMLINUX_SYMBOL_STR(spi_sync) },
	{ 0x31accddd, __VMLINUX_SYMBOL_STR(tty_register_device) },
	{ 0x63d766d, __VMLINUX_SYMBOL_STR(tty_unregister_device) },
	{ 0x67b27ec1, __VMLINUX_SYMBOL_STR(tty_std_termios) },
	{ 0x1000e51, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0x5037b0db, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0xe41e1e64, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x2ba8937e, __VMLINUX_SYMBOL_STR(tty_unregister_driver) },
	{ 0xc2c5b2b6, __VMLINUX_SYMBOL_STR(vsnprintf) },
	{ 0x4215a929, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0x9e52ac12, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xaadaa117, __VMLINUX_SYMBOL_STR(__tty_alloc_driver) },
	{ 0x653e67e7, __VMLINUX_SYMBOL_STR(of_find_node_by_name) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x35fd504a, __VMLINUX_SYMBOL_STR(tty_port_link_device) },
	{ 0x9d669763, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0xbd06913b, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0x53f0eeb1, __VMLINUX_SYMBOL_STR(kobject_init) },
	{ 0x4be7fb63, __VMLINUX_SYMBOL_STR(up) },
	{ 0x98dfb43, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x695a865d, __VMLINUX_SYMBOL_STR(tty_flip_buffer_push) },
	{ 0xdde438b5, __VMLINUX_SYMBOL_STR(spi_unregister_device) },
	{ 0x948cf4ac, __VMLINUX_SYMBOL_STR(of_property_read_variable_u32_array) },
	{ 0x8e3de043, __VMLINUX_SYMBOL_STR(tty_set_termios) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "8330D36AEC07919C7B695BF");
