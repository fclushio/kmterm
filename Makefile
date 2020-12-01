MODULENAME := kmterm

ifndef KDIR
KDIR := /lib/modules/`uname -r`/build
endif

ifndef PWD
PWD := `pwd`
endif


EXTRA_CFLAGS := -std=gnu99

.PHONY: all check disclean ignore

${MODULENAME}-objs := config.o core.o driver.o io.o log.o spi.o terminal.o tty.o driver/ili9225.o font/font.o font/basic.o
obj-m := $(MODULENAME).o

all:
	$(MAKE) -C $(KDIR) M=`pwd` modules

clean:
	@rm -rf *.o *.ko *.symvers *.order *.mod.c .*.cmd font/*.o  driver/*.o

