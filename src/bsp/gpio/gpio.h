#pragma once

/*USART 2*/
#define GPIO_USART2_REMAP_EN GPIO_Remap_USART2 //  0 for disable
#define GPIO_USART2_REMAP_MASK GPIO_Remap_USART2 //  0 for disable

#define GPIO_USART2_TX_PORT gpiod
#define GPIO_USART2_TX_PIN gpio_pin5

#define GPIO_USART2_RX_PORT gpiod
#define GPIO_USART2_RX_PIN gpio_pin6

/*NRF 24L01*/
#define GPIO_NRF1_CE_PORT gpioe
#define GPIO_NRF1_CE_PIN gpio_pin7

#define GPIO_NRF1_CS_PORT gpioe
#define GPIO_NRF1_CS_PIN gpio_pin8

#define GPIO_NRF1_IRQ_PORT gpioe
#define GPIO_NRF1_IRQ_PIN gpio_pin9

#define GPIO_SPI1_SCK_PORT gpioa
#define GPIO_SPI1_SCK_PIN gpio_pin5

#define GPIO_SPI1_MISO_PORT gpioa
#define GPIO_SPI1_MISO_PIN gpio_pin6

#define GPIO_SPI1_MOSI_PORT gpioa
#define GPIO_SPI1_MOSI_PIN gpio_pin7

#define GPIO_SPI1_REMAP_EN 0 //  0 for disable
#define GPIO_SPI1_REMAP_MASK 0 //  0 for disable

#define GPIO_SST25_CS_PORT gpioe
#define GPIO_SST25_CS_PIN gpio_pin6

#ifdef BOOT_PRJ
#define GPIO_IAP_PORT gpioc
#define GPIO_IAP_PIN gpio_pin8
#endif
