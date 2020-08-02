#ifndef __REG_H
#define __REG_H

#define CR1_UE_Set ((uint16_t)0x2000)   /*!< USART Enable Mask */
#define CR1_UE_Reset ((uint16_t)0xDFFF) /*!< USART Disable Mask */

#define CR1_WAKE_Mask ((uint16_t)0xF7FF) /*!< USART WakeUp Method Mask */

#define CR1_RWU_Set ((uint16_t)0x0002)   /*!< USART mute mode Enable Mask */
#define CR1_RWU_Reset ((uint16_t)0xFFFD) /*!< USART mute mode Enable Mask */
#define CR1_SBK_Set ((uint16_t)0x0001)   /*!< USART Break Character send Mask */
#define CR1_CLEAR_Mask ((uint16_t)0xE9F3) /*!< USART CR1 Mask */

/*!< USART Interrupt Enable Mask */
#define CR1_PCE_Set ((uint16_t)1 << 10)
#define CR1_PS_Set ((uint16_t)1 << 9)
#define CR1_PEIE_Set ((uint16_t)1 << 8)
#define CR1_RXNEIE_Set ((uint16_t)1 << 5)
#define CR1_IDLEIE_Set ((uint16_t)1 << 4)
#define CR1_TE_Set ((uint16_t)1 << 3)
#define CR1_RE_Set ((uint16_t)1 << 2)

#define CR2_Address_Mask ((uint16_t)0xFFF0) /*!< USART address Mask */

#define CR2_LINEN_Set ((uint16_t)0x4000)   /*!< USART LIN Enable Mask */
#define CR2_LINEN_Reset ((uint16_t)0xBFFF) /*!< USART LIN Disable Mask */

#define CR2_LBDL_Mask                                    \
  ((uint16_t)0xFFDF) /*!< USART LIN Break detection Mask \
                      */
#define CR2_STOP_CLEAR_Mask                                                   \
  ((uint16_t)0xCFFF)                            /*!< USART CR2 STOP Bits Mask \
                                                 */
#define CR2_CLOCK_CLEAR_Mask ((uint16_t)0xF0FF) /*!< USART CR2 Clock Mask */

#define CR3_DMAT_Set ((uint16_t)1 << 7)      /*!<DMA enable transmitter */
#define CR3_DMAT_Reset (~((uint16_t)1 << 7)) /*!< DMA enable transmitter*/

#define CR3_DMAR_Set ((uint16_t)1 << 6)      /*!<DMA enable receiver */
#define CR3_DMAR_Reset (~((uint16_t)1 << 6)) /*!< DMA enable receiver*/

#define CR3_EIE_Set ((uint16_t)1 << 0)      /*!<Error interrupt enable */
#define CR3_EIE_Reset (~((uint16_t)1 << 0)) /*!< Error interrupt enable*/

#define GTPR_LSB_Mask ((uint16_t)0x00FF) /*!< Guard Time Register LSB Mask */
#define GTPR_MSB_Mask ((uint16_t)0xFF00) /*!< Guard Time Register MSB Mask */
#define IT_Mask ((uint16_t)0x001F)       /*!< USART Interrupt Mask */

#endif
