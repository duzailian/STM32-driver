#pragma once

#define DEF_INT_CH(ch) en_##ch##_int

// interrupt of DMA for usart2
#define USART2_IRQ DEF_INT_CH(USART2)
#define USART2_PreemptPriority 5
#define USART2_SubPriority 0

// interrupt of  DMA for usart2
#define USART2_DMA_IRQ DEF_INT_CH(DMA1_Channel6)
#define DMA1_Channel6_PreemptPriority 5
#define DMA1_Channel6_SubPriority 1

#define NRF24L01_IRQ DEF_INT_CH(EXTI9_5)
#define EXTI9_5_PreemptPriority 6
#define EXTI9_5_SubPriority 1
