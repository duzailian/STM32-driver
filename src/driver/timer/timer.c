/***
 * @Author: 杜在连
 * @Date: 2021-07-18 22:02:38
 * @LastEditTime: 2021-12-29 21:07:16
 * @LastEditors: 杜在连
 * @Description:
 * @FilePath: /workspace/src/driver/timer/timer.c
 * @
 */
#include "include.h"

#include "reg.h"
#include "config.h"

#define InfoCh(timerx, channel)                                        \
  [timer_ch##channel] = {                                              \
      .reg = TIM##channel,                                             \
      .timer_ch = timer_ch##channel,                                   \
      .us_ccmr = TIMER##timerx##channel##_CCMR,                        \
      .us_ccer = TIMER##timerx##channel##_CCER,                        \
      .us_dier = TIMER##timerx##channel##_DIER,                        \
      .en_dma = TIMER##timerx##channel##_DMAx,                         \
      .en_dma_ch = TIMER##timerx##channel##_DMA_CH,                    \
      .us_cr2 = TIMER##timerx##channel##_CR2,                          \
      .st_gpio_cfg =                                                   \
          {                                                            \
              .port = TIM##timerx##channel##_GPIO_PORT,                \
              .pinx = TIM##timerx##channel##_GPIO_PIN,                 \
              .io_mode = TIM##timerx##channel##_GPIO_MODE,             \
              .cnf = TIM##timerx##channel##_GPIO_CNG,                  \
              .af_remap = TIM##timerx##channel##_GPIO_REMAP,           \
              .af_remap_mask = TIM##timerx##channel##_GPIO_REMAP_MASK, \
          },                                                           \
  }

#define Info(timerx)                            \
  [timer##timerx] = {                           \
      .reg = TIM##timerx,                       \
      .pst_info_ch = &ast_info##timerx##_ch[0], \
      .ul_ch_cnt = sizeof_array(ast_info1_ch),  \
      .us_cr1 = TIM##timerx##_CR1,              \
      .us_cr2 = TIM##timerx##_CR2,              \
      .us_smcr = TIM##timerx##_SMCR,            \
      .us_dier = TIM##timerx##_DIER,            \
      .us_inr = TIM##timerx##_INTERVAL,         \
      .us_arr = TIM##timerx##_ARR,              \
      .us_rcr = TIM##timerx##_RCR,              \
      .rcc_enr = RCC_TIM##timerx##_ENR,         \
      .rcc_reg = RCC_TIM##timerx##_REG,         \
  }

struct __st_info_ch_t;

typedef int (*init_func_t)(const struct __st_info_ch_t *);

typedef const struct __st_info_ch_t {
  TIM_TypeDef *reg;
  en_timer_ch_t timer_ch;
  uint16_t us_ccmr; // 写入CCMRx寄存器的值
  uint16_t us_ccer; // 写入CCERx寄存器的值
  uint16_t us_cr2; // 或到 CR2寄存器中的值
  uint16_t us_dier; // 或到 DIER寄存器中的值
  en_dma_t en_dma; // DMAx
  en_dma_ch_t en_dma_ch; // DMA channel
  st_gpio_cfg_t st_gpio_cfg; // GPIO 配置信息
} st_info_ch_t;

typedef const struct __st_inf_t {
  TIM_TypeDef *reg;
  st_info_ch_t *pst_info_ch;
  uint32_t ul_ch_cnt; // channel 个数
  uint16_t us_cr1; // CR1寄存器初始化值
  uint16_t us_cr2; // CR2寄存器初始化值
  uint16_t us_smcr; // SMCR 寄存器初始化值
  uint16_t us_dier; // DIER寄存器初始化值
  uint16_t us_inr; // counter加一的时间间隔(10us为单位)
  uint16_t us_arr; // ARR寄存器初始化值
  uint16_t us_rcr; // RCR寄存器初始化值
  uint32_t rcc_enr; // RCC ENR 位
  __IO uint32_t *rcc_reg; // RCC 配置寄存器地址
} st_info_t;

static int init_timer_as_ic(st_info_ch_t *st_info);

static st_info_ch_t ast_info1_ch[] = {
    InfoCh(1, 1),
};

static st_info_t ast_info[] = {
    Info(1),
};

#define rcc_init(reg, enr) \
  do {                     \
    *reg |= enr;           \
  } while (0)

static int __dma_init(st_info_ch_t *pst_info,
                      st_capture_info_t *pst_capture_info) {
  TIM_TypeDef *reg = pst_info->reg;
  st_dma_parm_t st_param;

  st_param.en_dma = pst_info->en_dma;
  st_param.en_dma_ch = pst_info->en_dma_ch;
  st_param.en_tr_dir = en_to_mem;
  st_param.per_addr = (uint32_t)((uint32_t *)&reg->CCR1 + pst_info->timer_ch);
  st_param.mem_addr = (uint32_t)pst_capture_info->pv_buffer;
  st_param.sz_len = pst_capture_info->sz_len;
  st_param.complete_cb = pst_capture_info->finish_cb;
  st_param.complete_cb_arg = pst_capture_info->pv_cb_para;

  return start_dma(&st_param);
}

/*
将某timer的某channel配置为输入捕获(input capture)
*/
static int init_timer_as_ic(st_info_ch_t *pst_info) {
  TIM_TypeDef *reg = pst_info->reg;
  en_timer_ch_t timer_ch = pst_info->timer_ch;

  if (timer_ch > timer_ch2) {
    reg->CCMR2 = (uint32_t)pst_info->us_ccmr;
  } else {
    reg->CCMR1 = (uint32_t)pst_info->us_ccmr;
  }
  reg->CCER = (uint32_t)pst_info->us_ccer;
  reg->CR2 |= pst_info->us_cr2;
  reg->DIER |= pst_info->us_dier;

  init_gpio(&pst_info->st_gpio_cfg);
  return 0;
}

static uint32_t get_psc(TIM_TypeDef *reg, uint16_t us_inr) {
  RCC_ClocksTypeDef RCC_Clocks;
  uint32_t ul_freq = 0;
  uint32_t ul_ret = 0;

  RCC_GetClocksFreq(&RCC_Clocks); // 获取当前时钟频率
#ifdef STM32F10X_CL
  if (TIM1 == reg)
    ul_freq = RCC_Clocks.PCLK2_Frequency;
  else
    ul_freq = RCC_Clocks.PCLK1_Frequency;
#else
  static const TIM_TypeDef *rcc_regs[] = {
      TIM1, TIM8, TIM9, TIM10, TIM11,
  };
  ul_freq = RCC_Clocks.PCLK1_Frequency;
  for (size_t i = 0; i < sizeof_array(rcc_regs); i++) {
    if (rcc_regs[i] == reg)
      ul_freq = RCC_Clocks.PCLK2_Frequency;
  }
#endif
  ul_ret = us_inr * (ul_freq / MHZ) - 1;
  if (ul_ret > USHRT_MAX)
    printf("timer psc out of range:%ld!\r\n", ul_ret);
  return ul_ret;
}

static int __reg_init(st_info_t *pst_info) {
  TIM_TypeDef *reg = pst_info->reg;

  rcc_init(pst_info->rcc_reg, pst_info->rcc_enr);
  reg->CR1 = 0;

  reg->CR1 = pst_info->us_cr1;
  reg->CR2 = pst_info->us_cr2;
  reg->SMCR = pst_info->us_smcr;
  reg->DIER = pst_info->us_dier;
  reg->PSC = get_psc(reg, pst_info->us_inr);
  reg->ARR = pst_info->us_arr;
  reg->RCR = pst_info->us_rcr;
  reg->CNT = 0;

  reg->CR1 |= CR1_CEN;

  return 0;
}

extern int timer_init(void) {
  for (size_t i = 0; i < sizeof_array(ast_info); i++) {
    __reg_init(&ast_info[i]);
  }
  return 0;
}

extern int start_capture(st_capture_info_t *pst_capture_info) {
  st_info_t *pst_info = &ast_info[pst_capture_info->en_timerx];
  st_info_ch_t *pst_info_ch = &pst_info->pst_info_ch[pst_capture_info->en_ch];
  int ret = 0;

  ret = init_timer_as_ic(pst_info_ch);
  if (ret < 0)
    printf("timer%xch%d capture init error!\r\n",
           pst_capture_info->en_timerx + 1, pst_capture_info->en_ch + 1);
  __dma_init(pst_info_ch, pst_capture_info);
  if (ret < 0)
    printf("timer%xch%d DMA init error!\r\n", pst_capture_info->en_timerx + 1,
           pst_capture_info->en_ch + 1);
  return 0;
}

#if TIM_DBG
#if 0
extern void test_timer(void) {
  uint32_t ul_pre = TIM1->CNT;
  uint32_t ul_now = TIM1->CNT;

  printf("i2c test start!\r\n");

  while (1) {
    ul_now = TIM1->CNT;
    if (ul_now < (ul_pre + 100))
      continue;
    printf("i2c testing!\r\n");
    ul_pre = ul_now;
  }
}
#else
extern void test_timer(void) {
  st_capture_info_t st_capture_info = {
    .en_timerx = timer1,
    .en_ch = timer_ch1,
    };

  start_capture(&st_capture_info);
  while(1);
}
#endif
#endif