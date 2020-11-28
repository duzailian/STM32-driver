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
            ],
            "define": ["STM32F10X_CL", "USE_STDPERIPH_DRIVER", "DEBUG_EN"],
            "icf": {
                "symbol __ICFEDIT_region_ROM_start__": "0x08000000",
                "symbol __ICFEDIT_region_ROM_end__":"0x0800ffff",
                "symbol __ICFEDIT_size_heap__":"0",
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
                'src/driver/systick/systick.c',
                'src/driver/usart/usart.c',
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
                "symbol __ICFEDIT_region_ROM_start__": "0x08010000",
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
