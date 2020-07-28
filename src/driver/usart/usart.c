#include "include.h"
#include "reg.h"

typedef const struct {
	gpio_cfg_t rx;
	gpio_cfg_t tx;
} st_gpio_t;

typedef const struct {
	USART_TypeDef *USARTx;
	st_gpio_t st_gpio;
	uint32_t ul_rcc_enr;
	uint32_t ul_bandrate;
} st_info_t;

#define TX_GPIO_IOMODE gpio_output_2M
#define RX_GPIO_IOMODE gpio_input

#define TX_GPIO_MODE gpio_af_opp
#define RX_GPIO_MODE gpio_float

#define GPIO_INFO(port, __mode)                                                \
	{                                                                      \
		.gpiox = GPIO_USART##port##_##__mode##_PORT,                   \
		.pinx = GPIO_USART##port##_##__mode##_PIN,                     \
		.io_mode = __mode##_GPIO_IOMODE, .mode = __mode##_GPIO_MODE,   \
	}

#define INFO(ch)                                                                              \
	{                                                                                     \
		.USARTx = USART##ch,                                                       \
    .st_gpio =                                                                 \
        {                                                                      \
            .rx = GPIO_INFO(ch, RX),                                           \
            .tx = GPIO_INFO(ch, TX),                                           \
        },                                                                     \
    .ul_rcc_enr = RCC_APB1ENR(USART##ch), .ul_bandrate = USART##ch##_BAUDRATE, \
	}

static st_info_t ast_info[] = {
	INFO(2),
};

static void __init_reg(st_info_t *pst_info)
{
	USART_TypeDef *USARTx = pst_info->USARTx;
	uint32_t fck = 0;
	RCC_ClocksTypeDef RCC_Clocks;

	RCC_GetClocksFreq(&RCC_Clocks);
	if (USARTx == USART1)
		fck = RCC_Clocks.PCLK2_Frequency;
	else
		fck = RCC_Clocks.PCLK1_Frequency;

	USARTx->CR1 = CR1_PCE_Set | CR1_IDLEIE_Set;
	USARTx->CR3 = CR3_DMAT_Set | CR3_DMAR_Set;
	USARTx->BRR = fck / pst_info->ul_bandrate;
}

static void __init_gpio(st_gpio_t *pst_info)
{
	return;
}
static void inline __init_rcc(st_info_t *pst_info)
{
	if (pst_info->USARTx == USART1)
		RCC->APB2ENR |= RCC_APB2ENR(USART1);
	else
		RCC->APB1ENR |= pst_info->ul_rcc_enr;

	return;
}
static void init_channel(st_info_t *pst_info)
{
	__init_rcc(pst_info);
	__init_reg(pst_info);
	__init_gpio(&pst_info->st_gpio);
	return;
}

extern void init_usart(void)
{
	int i = 0;

	for (i = 0; i < sizeof_array(ast_info); i++) {
		init_channel(&ast_info[i]);
	}
	return;
}
extern void usart_send(en_usart_t ch, uint8_t *puc_data, size_t sz_len)
{
	return;
}

#if USART_DBG
void usart_dug(void)
{
	return;
}
#endif
