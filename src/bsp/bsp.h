#pragma once

#include "cfg/cfg.h"
#include "common.h"
#include "dma/dma.h"
#include "gpio/gpio.h"
#include "interrupt/interrupt.h"
#include "nrf24l01/nrf24l01.h"
#include "rcc/rcc.h"
#include "spi/spi.h"
#include "usart/usart.h"
#include "exti/exti.h"
#include "ext_flash/ext_flash.h"

extern void init_bsp(void);
#if BSP_DBG
extern void bsp_dbg(void);
#endif