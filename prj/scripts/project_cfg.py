Projects = {
    "firmware": [
        {
            'name':
            'comm-setting',
            'include': [
                "src/app",
                "src/bsp",
                "src/driver",
                "src/lib",
                "src/os",
                "src/os/ucos/cfg/",
                "src/os/ucos/Task/",
                "src/os/ucos/uC-CPU/",
                "src/os/ucos/uC-CPU/ARM-Cortex-M3/IAR/",
                "src/os/ucos/uC-LIB/",
                "src/os/ucos/uCOS-III/Ports/ARM-Cortex-M3/Generic/IAR",
                "src/os/ucos/uCOS-III/Source/",
                "src/lib/ST/inc",
            ],
            "define": ["STM32F10X_CL", "USE_STDPERIPH_DRIVER", "DEBUG_EN"],
            "icf": {
                # must match with "__ICFEDIT_region_ROM_end__" of boot project
                # and VECT_TAB_OFFSET in the code
                # and "__ICFEDIT_intvec_start__"
                "symbol __ICFEDIT_region_ROM_start__": "0x08005000",
                "symbol __ICFEDIT_intvec_start__":"0x08005000",
                "symbol __ICFEDIT_size_heap__": "0",
            },
        },
        {
            'name': 'app',
            'src': [
                'src/app/main.c',
            ],
        },
        {
            'name': 'bsp',
            'src': [
                'src/bsp/bsp.c',
                'src/bsp/nrf24l01/nrf24l01.c',
            ],
        },
        {
            'name':
            'driver',
            'src': [
                'src/driver/driver.c',
                'src/driver/dma/dma.c',
                'src/driver/exti/exti.c',
                'src/driver/gpio/gpio.c',
                'src/driver/interrupt/interrupt.c',
                'src/driver/log/log.c',
                'src/driver/rcc/rcc.c',
                'src/driver/spi/spi.c',
                'src/driver/usart/usart.c',
            ],
        },
        {
            'name':
            'lib',
            'src': [
                'src/lib/CMSIS/CM3/CoreSupport/core_cm3.c',
                'src/lib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/iar/startup_stm32f10x_cl.s',
                'src/lib/library/crc8/crc8.c',
                'src/lib/ST/src/stm32f10x_it.c',
                'src/lib/ST/src/stm32f10x_rcc.c',
                'src/lib/ST/src/system_stm32f10x.c',
            ],
        },
        {
            'name':
            'os',
            'src': [
                'src/os/os.c',
                'src/os/ucos/Bsp/os_bsp.c',
                'src/os/ucos/Task/task.c',
                'src/os/ucos/uC-CPU/cpu_core.c',
                'src/os/ucos/uC-CPU/ARM-Cortex-M3/IAR/cpu_a.asm',
                'src/os/ucos/uC-CPU/ARM-Cortex-M3/IAR/cpu_c.c',
                'src/os/ucos/uC-LIB/lib_ascii.c',
                'src/os/ucos/uC-LIB/lib_math.c',
                'src/os/ucos/uC-LIB/lib_mem.c',
                'src/os/ucos/uC-LIB/lib_str.c',
                'src/os/ucos/uC-LIB/Ports/ARM-Cortex-M3/IAR/lib_mem_a.asm',
                'src/os/ucos/uCOS-III/Ports/ARM-Cortex-M3/Generic/IAR/os_cpu_a.asm',
                'src/os/ucos/uCOS-III/Ports/ARM-Cortex-M3/Generic/IAR/os_cpu_c.c',
                'src/os/ucos/uCOS-III/Source/os_cfg_app.c',
                'src/os/ucos/uCOS-III/Source/os_core.c',
                'src/os/ucos/uCOS-III/Source/os_dbg.c',
                'src/os/ucos/uCOS-III/Source/os_flag.c',
                'src/os/ucos/uCOS-III/Source/os_int.c',
                'src/os/ucos/uCOS-III/Source/os_mem.c',
                'src/os/ucos/uCOS-III/Source/os_msg.c',
                'src/os/ucos/uCOS-III/Source/os_mutex.c',
                'src/os/ucos/uCOS-III/Source/os_pend_multi.c',
                'src/os/ucos/uCOS-III/Source/os_prio.c',
                'src/os/ucos/uCOS-III/Source/os_q.c',
                'src/os/ucos/uCOS-III/Source/os_sem.c',
                'src/os/ucos/uCOS-III/Source/os_stat.c',
                'src/os/ucos/uCOS-III/Source/os_task.c',
                'src/os/ucos/uCOS-III/Source/os_tick.c',
                'src/os/ucos/uCOS-III/Source/os_time.c',
                'src/os/ucos/uCOS-III/Source/os_tmr.c',
                'src/os/ucos/uCOS-III/Source/os_var.c',
            ],
        },
    ],
    "boot": [
        {
            'name':
            'comm-setting',
            'include': [
                "src/app",
                "src/bsp",
                "src/driver",
                "src/lib",
                "src/os",
                "src/os/ucos/cfg/",
                "src/os/ucos/Task/",
                "src/os/ucos/uC-CPU/",
                "src/os/ucos/uC-CPU/ARM-Cortex-M3/IAR/",
                "src/os/ucos/uC-LIB/",
                "src/os/ucos/uCOS-III/Ports/ARM-Cortex-M3/Generic/IAR",
                "src/os/ucos/uCOS-III/Source/",
                "src/lib/ST/inc",
            ],
            "define":
            ["STM32F10X_CL", "USE_STDPERIPH_DRIVER", "DEBUG_EN", "BOOT_PRJ"],
            "icf": {
                "symbol __ICFEDIT_region_ROM_start__": "0x08000000",
                "symbol __ICFEDIT_region_ROM_end__": "0x08004fff",
                "symbol __ICFEDIT_size_heap__": "0x00"
            },
        },
        {
            'name': 'app',
            'src': [
                'src/app/iap/iap.c',
                'src/app/main.c',
            ],
        },
        {
            'name': 'bsp',
            'src': [
                'src/bsp/bsp.c',
                'src/bsp/nrf24l01/nrf24l01.c',
            ],
        },
        {
            'name':
            'driver',
            'src': [
                'src/driver/driver.c',
                'src/driver/dma/dma.c',
                'src/driver/exti/exti.c',
                'src/driver/gpio/gpio.c',
                'src/driver/interrupt/interrupt.c',
                'src/driver/log/log.c',
                'src/driver/rcc/rcc.c',
                'src/driver/spi/spi.c',
                'src/driver/systick/systick.c',
                'src/driver/usart/usart.c',
            ],
        },
        {
            'name':
            'lib',
            'src': [
                'src/lib/CMSIS/CM3/CoreSupport/core_cm3.c',
                'src/lib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/iar/startup_stm32f10x_cl.s',
                'src/lib/library/crc8/crc8.c',
                'src/lib/ST/src/stm32f10x_it.c',
                'src/lib/ST/src/stm32f10x_rcc.c',
                'src/lib/ST/src/system_stm32f10x.c',
            ],
        },
        {
            'name': 'os',
            'src': [
                'src/os/ucos/uC-CPU/ARM-Cortex-M3/IAR/cpu_a.asm',
            ],
        },
    ]
}
iar_ogcmenu = "STM32F107VC ST STM32F107VC"
global_ldflags = "--config platform/board/stm32f103rb-nucleo/stm32f103xb_flash.icf --silent"
