#pragma once

/**********************************CR1**********************************/
/*ARPE Auto-reload preload enable*/
#define CR1_ARPD (0) // Auto-reload preload disable
#define CR1_ARPE (1 << 7) // Auto-reload preload enable

/*Center-aligned mode selection*/
#define CR1_CMS_EDGE (0) // Edge-aligned mode.
#define CR1_CMS_CENTER1 (1 << 5) // Center-aligned mode 1.
#define CR1_CMS_CENTER2 (2 << 5) // Center-aligned mode 2.
#define CR1_CMS_CENTER3 (3 << 5) // Center-aligned mode 3

/*counter direction*/
#define CR1_DIR_UP (0 << 4)
#define CR1_DIR_DOWN (1 << 4)

/* One pulse mode*/
#define CR1_OPM_DIS (0 << 3)
#define CR1_OPM_EN (1 << 3)

/* Update request source*/
/*Only counter overflow/underflow generates an update interrupt or DMA request
 * if enabled.*/
#define CR1_URS (1 << 2)

/*Update disable*/
/*UEV disabled*/
#define CR1_UDIS (1 << 1)

/*Counter enable*/
#define CR1_CEN (1 << 0)

/**********************************CR2**********************************/
/*TI1 selection*/
#define CR2_TI1S (1 << 7)

/*Master mode selection*/
#define CR2_MMS_RESET (0 << 4)
#define CR2_MMS_ENABLE (1 << 4)
#define CR2_MMS_UPDATE (2 << 4)
#define CR2_MMS_COMPARE_PULSE (3 << 4)
#define CR2_MMS_COMPARE_OC1REF (4 << 4)
#define CR2_MMS_COMPARE_OC2REF (5 << 4)
#define CR2_MMS_COMPARE_OC3REF (6 << 4)
#define CR2_MMS_COMPARE_OC4REF (7 << 4)

/*Capture/compare DMA selection*/
#define CR2_CCx 0 // CCx DMA request sent when CCx event occurs
#define CR2_UEV (1 << 3) // CCx DMA requests sent when update event occurs

/*Capture/compare control update selection*/
#define CR2_CCUS (1 << 2)

/*Capture/compare preloaded control*/
#define CR2_CCPC (1 << 0) // CCxE, CCxNE and OCxM bits are preloaded,

/**********************************SMCR**********************************/
#define SMCR_SMS_DIS (0 << 0)
#define SMCR_SMS_ENCODE1 (1 << 0)
#define SMCR_SMS_ENCODE2 (2 << 0)
#define SMCR_SMS_ENCODE3 (3 << 0)
#define SMCR_SMS_RESET (4 << 0)
#define SMCR_SMS_GATE (5 << 0)
#define SMCR_SMS_TRIGGER (6 << 0)
#define SMCR_SMS_EXTERN (7 << 0)

#define SMCR_TS_ITR0 (0 << 4)
#define SMCR_TS_ITR1 (1 << 4)
#define SMCR_TS_ITR2 (2 << 4)
#define SMCR_TS_ITR3 (3 << 4)
#define SMCR_TS_TI1F_ED (4 << 4)
#define SMCR_TS_TI1FP1 (5 << 4)
#define SMCR_TS_TI1FP2 (6 << 4)
#define SMCR_TS_ETRF (7 << 4)

#define SMCR_MSM_DIS (0 << 7)
#define SMCR_MSM_EN (1 << 7)

#define SMCR_ETF_DTS (0 << 8)
#define SMCR_ETF_CKINT_N2 (1 << 8)
#define SMCR_ETF_CKINT_N4 (2 << 8)
#define SMCR_ETF_CKINT_N8 (3 << 8)
#define SMCR_ETF_DTS2_N6 (4 << 8)
#define SMCR_ETF_DTS2_N8 (5 << 8)
#define SMCR_ETF_DTS4_N6 (6 << 8)
#define SMCR_ETF_DTS4_N8 (7 << 8)
#define SMCR_ETF_DTS8_N6 (8 << 8)
#define SMCR_ETF_DTS8_N8 (9 << 8)
#define SMCR_ETF_DTS16_N5 (10 << 8)
#define SMCR_ETF_DTS16_N6 (11 << 8)
#define SMCR_ETF_DTS16_N8 (12 << 8)
#define SMCR_ETF_DTS32_N5 (13 << 8)
#define SMCR_ETF_DTS32_N6 (14 << 8)
#define SMCR_ETF_DTS32_N8 (15 << 8)

#define SMCR_ETPS_NO_PSC (0 << 12)
#define SMCR_ETPS_ETRP_DIV2 (1 << 12)
#define SMCR_ETPS_ETRP_DIV4 (2 << 12)
#define SMCR_ETPS_ETRP_DIV8 (3 << 12)

#define SMCR_ECE (1 << 14)

#define SCMR_ETP (1 << 15)

/**********************************SMCR**********************************/
#define DIER_UIE (1 << 0)

#define DIER_CC1IE (1 << 1)
#define DIER_CC2IE (1 << 2)
#define DIER_CC3IE (1 << 3)
#define DIER_CC4IE (1 << 4)

#define DIER_COMIE (1 << 5)
#define DIER_TIE (1 << 6)
#define DIER_BIE (1 << 7)
#define DIER_UDE (1 << 8)

#define DIER_CC1DE (1 << 9)
#define DIER_CC2DE (1 << 10)
#define DIER_CC3DE (1 << 11)
#define DIER_CC4DE (1 << 12)


#define DIER_COMDE (1 << 13)
#define DIER_TDE (1 << 14)

/**********************************CCMR**********************************/
#define CCMR_BITS_SZ (8) // n bits for each channel in CCMR

/*CCMR ICxF*/
#define CCMR_ICxF_DTS1_N0 (0 << 4) // No filter, sampling is done at fDTS
#define CCMR_ICxF_CK_INT_N2 (1 << 4) // fSAMPLING=fCK_INT, N=2
#define CCMR_ICxF_CK_INT_N4 (2 << 4) // fSAMPLING=fCK_INT, N=4
#define CCMR_ICxF_CK_INT_N8 (3 << 4) // fSAMPLING=fCK_INT, N=8
#define CCMR_ICxF_DTS2_N6 (4 << 4) // fSAMPLING=fDTS/2, N=6
#define CCMR_ICxF_DTS2_N8 (5 << 4) //  fSAMPLING=fDTS/2, N=8
#define CCMR_ICxF_DTS4_N6 (6 << 4) // fSAMPLING=fDTS/4, N=6
#define CCMR_ICxF_DTS4_N8 (7 << 4) // fSAMPLING=fDTS/4, N=8
#define CCMR_ICxF_DTS8_N6 (8 << 4) // fSAMPLING=fDTS/8, N=6
#define CCMR_ICxF_DTS8_N8 (9 << 4) // fSAMPLING=fDTS/8, N=8
#define CCMR_ICxF_DTS16_N5 (10 << 4) // fSAMPLING=fDTS/16, N=5
#define CCMR_ICxF_DTS16_N6 (11 << 4) // fSAMPLING=fDTS/16, N=6
#define CCMR_ICxF_DTS16_N8 (12 << 4) // fSAMPLING=fDTS/16, N=8
#define CCMR_ICxF_DTS32_N5 (13 << 4) // fSAMPLING=fDTS/32, N=5
#define CCMR_ICxF_DTS32_N6 (14 << 4) // fSAMPLING=fDTS/32, N=6
#define CCMR_ICxF_DTS32_N8 (15 << 4) // fSAMPLING=fDTS/32, N=8

/*CCMR ICxPSC*/
#define CCMR_ICxPSC_NO (0) // no prescaler
#define CCMR_ICxPSC_2E (1 << 2) // capture is done once every 2 events
#define CCMR_ICxPSC_4E (2 << 2) // capture is done once every 4 events
#define CCMR_ICxPSC_8E (3 << 2) // capture is done once every 8 events

/*CCMR ICxS*/
/*Capture/Compare x Selection*/
#define CCMR_CCxS_OUTPUT (0 << 0)
#define CCMR_CCxS_INPUT_MAP1 (1 << 0)
#define CCMR_CCxS_INPUT_MAP2 (2 << 0)
#define CCMR_CCxS_INPUT_MAP_TRC (3 << 0)

/**********************************CCER**********************************/
#define CCER_BITS_SZ (4) // n bits for each channel in CCER

#define CCER_CCxE (1 << 0) // Capture/Compare 1 output enable

/*inverted: capture is done on a falling edge of IC1*/
#define CCER_CCxP (1 << 1)

/*Capture/Compare 1 complementary output enable*/
#define CCER_CCxNE (1 << 2)

#define CCER_CCxNP (1 << 3) // OC1N active low.

/**********************************DCR**********************************/
#define DBLnTR(num) ((num - 1) << 8)

/**********************************RCC**********************************/
#define RCC_TIM1_ENR RCC_ENR(APB2, TIM1)

#define RCC_TIM1_REG (&RCC->APB2ENR)

#define RCC_TIM2_ENR RCC_ENR(APB1, TIM2)

#define RCC_TIM2_REG (&RCC->APB1ENR)