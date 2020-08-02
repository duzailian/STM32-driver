#include "include.h"

#define DMA_CHANNEL_SZ 7  // number of DMA  channels

#define CCR_TEIE (1 << 3)  //  Transfer error interrupt enable

#define CCR_TCIE (1 << 1)  //  Transfer complete interrupt enable

#define CCR_EN (1 << 0)  //  Channel enable

typedef struct {
  DMA_Channel_TypeDef *DMA_chx;
  st_int_parm_t st_int_parm;
} st_channel_t;

typedef const struct {
  DMA_TypeDef *DMAx;
  st_channel_t *DMA_chs[DMA_CHANNEL_SZ];
  uint32_t ul_rcc_enr;
} st_info_t;

#define INFO(dmax, ch)                                            \
  {                                                               \
    .DMAx = DMA##dmax, .DMA_chs[en_dma_ch##ch] = &st_channel##ch, \
    .ul_rcc_enr = RCC_AHBENR_DMA##dmax##EN                        \
  }

#define ADD_CHANNEL(dmax, ch)                    \
  static st_channel_t st_channel##ch = {         \
      .DMA_chx = DMA##dmax##_Channel##ch,        \
      .st_int_parm = {.en_int = USART2_DMA_IRQ}, \
  };

ADD_CHANNEL(1, 6)

static st_info_t ast_info[] = {
    INFO(1, 6),
};

static int ISR(void *__self) {
  st_info_t *pst_info = (st_info_t *)__self;
  return 0;
}

#define init_int(pst_ch, pst_info)                     \
  do {                                                 \
    st_int_parm_t int_parameter;                       \
    int_parameter.en_int = pst_ch->st_int_parm.en_int; \
    int_parameter.func = ISR;                          \
    int_parameter.p_arg = (void *)pst_info;            \
    reg_int_func(&int_parameter);                      \
  } while (0)

static void int_reg(const st_dma_parm_t *pst_param,
                    DMA_Channel_TypeDef *DMA_chx) {
  uint32_t ul_tmp = 0;

  ul_tmp = DMA_Priority_High | DMA_MemoryDataSize_Byte |
           DMA_PeripheralDataSize_Byte | DMA_MemoryInc_Enable |
           DMA_PeripheralInc_Disable | DMA_Mode_Normal | CCR_TEIE | CCR_TCIE;

  if (en_from_mem == pst_param->en_tr_dir) {
    ul_tmp |= DMA_DIR_PeripheralDST;
  }
  DMA_chx->CCR = ul_tmp;
  DMA_chx->CNDTR = pst_param->us_len;
  DMA_chx->CPAR = pst_param->per_addr;
  DMA_chx->CMAR = pst_param->mem_addr;

  return;
}

#define __init_rcc(enr) \
  do {                  \
    RCC->AHBENR |= enr; \
  } while (0)

int start_dma(const st_dma_parm_t *pst_param) {
  en_dma_t en_dma = pst_param->en_dma;
  en_dma_ch_t en_dma_ch = pst_param->en_dma_ch;
  st_info_t *pst_info = NULL;
  st_channel_t *pst_ch = NULL;

  if (en_dma >= sizeof_array(ast_info) ||
      (NULL == ast_info[en_dma].DMA_chs[en_dma_ch])) {
    LOG_ERR("DMA init parameters error!");
    goto Error;
  }
  pst_info = &ast_info[en_dma];
  pst_ch = pst_info->DMA_chs[en_dma_ch];

  pst_ch->DMA_chx->CCR &= ~CCR_EN;  // disable dma
  __init_rcc(pst_info->ul_rcc_enr);
  init_int(pst_ch, pst_info);  //
  int_reg(pst_param, pst_ch->DMA_chx);
  pst_ch->DMA_chx->CCR |= CCR_EN;  // enable dma
  return 0;
Error:
  return -1;
}

int get_transterred_size(const st_dma_parm_t *pst_param) {
  en_dma_t en_dma = pst_param->en_dma;
  en_dma_ch_t en_dma_ch = pst_param->en_dma_ch;
  st_info_t *pst_info = NULL;
  st_channel_t *pst_ch = NULL;

  if (en_dma >= sizeof_array(ast_info) ||
      (NULL == ast_info[en_dma].DMA_chs[en_dma_ch])) {
    LOG_ERR("DMA init parameters error!");
    goto Error;
  }

  pst_info = &ast_info[en_dma];
  pst_ch = pst_info->DMA_chs[en_dma_ch];

  return pst_param->us_len - pst_ch->DMA_chx->CNDTR;
Error:
  return -1;
}
