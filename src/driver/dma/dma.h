#ifndef __DRV_DMA_H
#define __DRV_DMA_H

typedef enum {
  en_dma1,
  en_dma2,
} en_dma_t;

typedef enum {
  en_dma_ch1,
  en_dma_ch2,
  en_dma_ch3,
  en_dma_ch4,
  en_dma_ch5,
  en_dma_ch6,
  en_dma_ch7,
} en_dma_ch_t;
typedef enum { // transfer direction
  en_from_mem, // from memory to peripheral
  en_to_mem, // from memory to peripheral
} en_tr_dir_t;

typedef struct {
  en_dma_t en_dma;
  en_dma_ch_t en_dma_ch;

  en_tr_dir_t en_tr_dir;// transmit direction
  uint32_t per_addr; // peripheral address
  uint32_t mem_addr; // memory address
  size_t sz_len; // data length

  int_func_t complete_cb;
  void *complete_cb_arg;
} st_dma_parm_t;

extern int start_dma(const st_dma_parm_t *pst_param);
extern int get_transterred_size(const st_dma_parm_t *pst_param);

#endif
