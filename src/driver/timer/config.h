#pragma once

#define TIM1_

// FIXME: 确认该文件中的所有宏定义是否满足设计需要
/**************************TIM1 REG********************************/
#define TIM1_CR1                                                             \
  (TIM_CKD_DIV1 /*Clock division(CKD) tDTS=tCK_INT*/                         \
   | CR1_ARPD /*TIMx_ARR register is not buffered*/                          \
   | CR1_CMS_EDGE /* Edge-aligned mode.*/                                    \
   | CR1_DIR_UP /*Counter used as upcounter*/                                \
   | CR1_OPM_DIS /*Counter is not stopped at update event*/                  \
   | CR1_URS /*Only counter overflow/underflow generates an update interrupt \
                or DMA request if enabled*/                                  \
  )

#define TIM1_CR2 (CR2_CCUS | CR2_CCPC)
#define TIM1_SMCR (SMCR_TS_TI1FP1 | SMCR_SMS_RESET)

#define TIM1_DIER (0)

#define TIM1_INTERVAL (1)// 以10us为单位的分频值

#define TIM1_ARR (0xffff)

#define TIM1_RCR (0)

/**************************TIM1 channel1 REG********************************/
#define TIMER11_CR2 (0)

#define TIMER11_DIER (DIER_CC1DE | DIER_CC2DE)

#define TIMER11_CCER (0)

#define TIMER11_CCMR                                                     \
  (TIM_ICSelection_DirectTI /*CC1 channel is configured as input, IC1 is \
                               mapped on TI1(CC1S)*/                     \
   | TIM_ICPSC_DIV1 /*no prescaler, capture is done each time an edge is \
                       detected on the capture input*/                   \
   | CCMR_ICxF_DTS1_N0 /*No filter, sampling is done at fDTS*/           \
  )

/**************************TIM1 channel2 REG********************************/
#define TIMER12_CCMR                                                       \
  (TIM_ICSelection_IndirectTI /*CC2 channel is configured as input, IC2 is \
                                 mapped on TI1(CC2S)*/                     \
   | TIM_ICPSC_DIV1 /*no prescaler, capture is done each time an edge is   \
                       detected on the capture input*/                     \
   | CCMR_ICxF_DTS1_N0 /*No filter, sampling is done at fDTS*/             \
  )

#define TIMER12_CCER (0)


/**************************TIM1 channel1 DMA********************************/
#define TIMER11_DMAx (en_dma1)
#define TIMER11_DMA_CH (en_dma_ch2)


/**************************TIM1 channel2 DMA********************************/
#define TIMER12_DMAx (en_dma1)
#define TIMER12_DMA_CH (en_dma_ch3)

/**************************TIM1 channel1 GPIO********************************/
#define TIM11_GPIO_PORT gpioa
#define TIM11_GPIO_PIN gpio_pin8
#define TIM11_GPIO_MODE gpio_input
#define  TIM11_GPIO_CNG gpio_float
#define  TIM11_GPIO_REMAP 0
#define  TIM11_GPIO_REMAP_MASK 0
