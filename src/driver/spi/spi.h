#pragma once

typedef enum {
  spi1,
  spi2,
  spi3,
} en_spi_t;

extern st_drv_if_t *open_spi(en_spi_t channel);

#if SPI_DBG
extern void test_spi(void);
#endif
