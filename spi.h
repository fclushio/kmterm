#ifndef KMTERM_SPI_H
#define KMTERM_SPI_H


#define KMTERM_HIGH  1
#define KMTERM_LOW   0
#define KMTERM_HIZ  -1

#define KMTERM_PIN_UNK -1

#include <linux/types.h>

/*
 * Depends on log, config
 */
int  kmterm_spi_init(void);
void kmterm_spi_exit(void);

void kmterm_gpio_rst_set(s8 _val);
void kmterm_gpio_dc_set (s8 _val);

int kmterm_spi_append(const void * _data, size_t _count);
int kmterm_spi_flush(void);



#endif // KMTERM_SPI_H
