#include "include.h"

typedef const struct {
	DMA_TypeDef *DMAx;
	DMA_Channel_TypeDef *DMA_ch_x;
	uint32_t ul_rcc_enr;
} st_info_t;

#define INFO_INIT()                                                            \
	{                                                                      \
	}

static st_info_t ast_info[] = {

}