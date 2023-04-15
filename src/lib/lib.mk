__lib_dir:=$(dir $(lastword $(MAKEFILE_LIST)))

inc-dirs+=$(__lib_dir)
inc-dirs+=$(__lib_dir)ST/inc

defs+=STM32F10X_HD # TODO: 将该选项移到kconfig中
defs+=USE_STDPERIPH_DRIVER# TODO: 将该选项移到kconfig中

src-y+=$(__lib_dir)lib.c

src-y+=$(__lib_dir)ST/src/stm32f10x_tim.c
src-y+=$(__lib_dir)ST/src/stm32f10x_rcc.c

src-y+=$(__lib_dir)printf/printf.c


undefine __lib_dir
