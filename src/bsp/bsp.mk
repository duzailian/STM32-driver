__bsp_dir:=$(dir $(lastword $(MAKEFILE_LIST)))

inc-dirs+=$(__bsp_dir)

src-y+=$(__bsp_dir)bsp.c
src-y+=$(__bsp_dir)ext_flash/ext_flash.c

undefine __bsp_dir
