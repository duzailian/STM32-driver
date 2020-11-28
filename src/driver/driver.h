#ifndef __DRIVER_H
#define __DRIVER_H

#include "list/list.h"
#include "cfg/cfg.h"
#include "interrupt/interrupt.h"
#include "dma/dma.h"
#include "gpio/gpio.h"
#include "log/log.h"
#include "rcc/rcc.h"
#include "spi/spi.h"
#include "tool/tool.h"
#include "usart/usart.h"
#include "exti/exti.h"
#ifdef BOOT_PRJ
#include "systick/systick.h"
#endif

extern void init_driver(void);
#if DRV_DBG
extern void drv_dbg(void);
#endif
#endif