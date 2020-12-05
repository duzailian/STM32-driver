#pragma once

typedef enum {
  spi1,
  spi2,
  spi3,
} en_spi_t;

typedef enum {
  spi_br0,  // SPI_BaudRatePrescaler_2
  spi_br1,  // SPI_BaudRatePrescaler_4
  spi_br2,  // SPI_BaudRatePrescaler_8
  spi_br3,  // SPI_BaudRatePrescaler_16
  spi_br4,  // SPI_BaudRatePrescaler_32
  spi_br5,  // SPI_BaudRatePrescaler_64
  spi_br6,  // SPI_BaudRatePrescaler_128
  spi_br7,  // SPI_BaudRatePrescaler_256
} en_spi_br_t;

#define SPI_CTRL 1
#define SPI_BAUD 2

extern st_drv_if_t *open_spi(en_spi_t channel);

#define spi_enable(self)            \
  do {                              \
    self->ioctl(self, SPI_CTRL, 1); \
  } while (0)
#define spi_disable(self)           \
  do {                              \
    self->ioctl(self, SPI_CTRL, 0); \
  } while (0)

/*baud in spi_set_baudrate() must be en_spi_br_t*/
#define spi_set_baudrate(self, baud) self->ioctl(self, SPI_BAUD, baud)

#if SPI_DBG
extern void test_spi(void);
#endif
