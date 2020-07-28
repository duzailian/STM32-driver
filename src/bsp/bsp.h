#ifndef __BSP_H
#define __BSP_H

#include "cfg/cfg.h"

#include "gpio/gpio.h"
#include "nrf24l01/nrf24l01.h"
#include "rcc/rcc.h"
#include "spi/spi.h"
#include "usart/usart.h"

extern void init_bsp(void);

#endif