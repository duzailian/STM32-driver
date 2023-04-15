__driver_dir:=$(dir $(lastword $(MAKEFILE_LIST)))

inc-dirs+=$(__driver_dir)

src-y+=$(__driver_dir)driver.c
src-y+=$(__driver_dir)timer/timer.c
src-y+=$(__driver_dir)dma/dma.c
src-y+=$(__driver_dir)exti/exti.c
src-y+=$(__driver_dir)flash/flash.c
src-y+=$(__driver_dir)gpio/gpio.c
src-y+=$(__driver_dir)interrupt/interrupt.c
src-y+=$(__driver_dir)log/log.c
src-y+=$(__driver_dir)rcc/rcc.c
src-y+=$(__driver_dir)spi/spi.c
src-y+=$(__driver_dir)systick/systick.c
src-y+=$(__driver_dir)usart/usart.c

undefine __driver_dir
