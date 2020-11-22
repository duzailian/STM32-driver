#pragma once

#define USART2_BAUDRATE 115200
#define USART2_BUFFER_NUM 10
#define USART2_BUFFER_SZ 1000

#define PRINT_USART usart2

#ifdef BOOT_PRJ
#define IAP_usart usart2
#endif
