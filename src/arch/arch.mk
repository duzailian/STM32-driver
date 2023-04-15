__arch_dir:=$(dir $(lastword $(MAKEFILE_LIST)))

src-y+=$(__arch_dir)cm3/startup/system_ARMCM3.c
asm-y+=$(__arch_dir)cm3/startup/startup_ARMCM3.S

CFLAGS+=-mcpu=cortex-m3 -mlittle-endian -mthumb -march=armv7-m
LDFLAGS+=-mcpu=cortex-m3 -mlittle-endian -mthumb -march=armv7-m

inc-dirs+=$(__arch_dir)cm3/inc
inc-dirs+=$(__arch_dir)core/inc

undefine __arch_dir
