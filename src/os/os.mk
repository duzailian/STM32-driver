__os_dir:=$(dir $(lastword $(MAKEFILE_LIST)))

src-y+=

inc-dirs+=$(__os_dir)
inc-dirs+=$(__os_dir)ucos/uC-CPU
inc-dirs+=$(__os_dir)ucos/cfg/
inc-dirs+=$(__os_dir)ucos/uC-CPU/ARM-Cortex-M3/GNU/
inc-dirs+=$(__os_dir)ucos/uC-LIB/
inc-dirs+=$(__os_dir)ucos/uCOS-III/Source/
inc-dirs+=$(__os_dir)ucos/uCOS-III/Ports/ARM-Cortex-M3/Generic/GNU/


src-y+=$(__os_dir)os.c
src-y+=$(__os_dir)ucos/Bsp/os_bsp.c
src-y+=$(__os_dir)ucos/Task/task.c

src-y+=$(__os_dir)ucos/uC-CPU/cpu_core.c
src-y+=$(__os_dir)ucos/uC-CPU/ARM-Cortex-M3/GNU/cpu_c.c
asm-y+=$(__os_dir)ucos/uC-CPU/ARM-Cortex-M3/GNU/cpu_a.s

src-y+=$(__os_dir)ucos/uC-LIB/lib_ascii.c
src-y+=$(__os_dir)ucos/uC-LIB/lib_math.c
src-y+=$(__os_dir)ucos/uC-LIB/lib_mem.c
src-y+=$(__os_dir)ucos/uC-LIB/lib_str.c

asm-y+=$(__os_dir)ucos/uCOS-III/Ports/ARM-Cortex-M3/Generic/GNU/os_cpu_a.s
src-y+=$(__os_dir)ucos/uCOS-III/Ports/ARM-Cortex-M3/Generic/GNU/os_cpu_c.c

src-y+=$(wildcard $(__os_dir)ucos/uCOS-III/Source/*.c)


undefine __os_dir
