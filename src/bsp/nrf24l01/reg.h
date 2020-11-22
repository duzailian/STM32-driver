#pragma once

#define MAX_REG_SZ 5
#define REG_ADDR_MASK (0x1f)

typedef enum {
  CONFIG = 0x00,
  EN_AA,
  EN_RXADDR,
  SETUP_AW,
  SETUP_RETR,
  RF_CH,
  RF_SETUP,
  STATUS,
  OBSERVE_TX,
  RPD,
  RX_ADDR_P0,
  RX_ADDR_P1,
  RX_ADDR_P2,
  RX_ADDR_P3,
  RX_ADDR_P4,
  RX_ADDR_P5,
  TX_ADDR,
  RX_PW_P0,
  RX_PW_P1,
  RX_PW_P2,
  RX_PW_P3,
  RX_PW_P4,
  RX_PW_P5,
  FIFO_STATUS = 0x17,

  DYNPD = 0x1c,
  FEATURE = 0x1d,
} en_reg_t;

#define MASK_RX_DR (1 << 6)
#define MASK_TX_DS (1 << 5)
#define MASK_MAX_RT (1 << 4)
#define EN_CRC (1 << 3)
#define CRCO (1 << 2)
#define PWR_UP (1 << 1)
#define PRIM_RX (1 << 0)

#define ENAA_P5 (1 << 5)
#define ENAA_P4 (1 << 4)
#define ENAA_P3 (1 << 3)
#define ENAA_P2 (1 << 2)
#define ENAA_P1 (1 << 1)
#define ENAA_P0 (1 << 0)

#define ERX_P5 (1 << 5)
#define ERX_P4 (1 << 4)
#define ERX_P3 (1 << 3)
#define ERX_P2 (1 << 2)
#define ERX_P1 (1 << 1)
#define ERX_P0 (1 << 0)

#define AW_Illegal 0x0
#define AW_3B 0x01
#define AW_4B 0x02
#define AW_5B 0x03

/*Auto Retransmit Delay*/
#define ARD_250 (0 << 4)
#define ARD_500 (1 << 4)
#define ARD_750 (2 << 4)
#define ARD_1000 (3 << 4)
#define ARD_1250 (4 << 4)
#define ARD_1500 (5 << 4)
#define ARD_1750 (6 << 4)
#define ARD_2000 (7 << 4)
#define ARD_2250 (8 << 4)
#define ARD_2500 (9 << 4)
#define ARD_2750 (10 << 4)
#define ARD_3000 (11 << 4)
#define ARD_3250 (12 << 4)
#define ARD_3500 (13 << 4)
#define ARD_3750 (14 << 4)
#define ARD_4000 (15 << 4)

/*Auto Retransmit Count*/
#define ARC_0 0
#define ARC_1 1
#define ARC_2 2
#define ARC_3 3
#define ARC_4 4
#define ARC_5 5
#define ARC_6 6
#define ARC_7 7
#define ARC_8 8
#define ARC_9 9
#define ARC_10 10
#define ARC_11 11
#define ARC_12 12
#define ARC_13 13
#define ARC_14 14
#define ARC_15 15

#define RF_CH_MASK ((1 << 7) - 1)

#define CONT_WAVE (1 << 7) // Enables continuous carrier transmit when high
/*data rates*/
#define RF_DR_2M (1 << 3)
#define RF_DR_1M (0)
#define RF_DR_250K (1 << 5)
#define RF_PWR_18DB (0 << 1)
#define RF_PWR_12DB (1 << 1)
#define RF_PWR_6DB (2 << 1)
#define RF_PWR_0DB (3 << 1)

#define RX_DR (1 << 6)
#define TX_DS (1 << 5)
#define MAX_RT (1 << 4)

#define RX_P_NO_POS (1)
#define RX_P_NO_MASK (0x07 << RX_P_NO_POS)

#define TX_FULL (1 << 0)

#define RX_FIFO_EMPTY (0x07)

#define PLOS_CNT_POS 4
#define PLOS_CNT_MASK (0x0f << PLOS_CNT_POS)
#define get_lost_packet_num(status) ((status & PLOS_CNT_MASK) >> PLOS_CNT_POS)

#define ARC_CNT_POS 0
#define ARC_CNT_MASK (0x0f << ARC_CNT_POS)
#define get_retrans_packets(status) ((status & ARC_CNT_MASK) >> ARC_CNT_POS)

// FIFO STATUS
#define TX_REUSE (1 << 6)
#ifndef TX_FULL
#define TX_FULL (1 << 5) // TX_FULL in FIFO STATUS register
#endif
#define TX_EMPTY (1 << 4)
#define RX_FULL (1 << 1)
#define RX_EMPTY (1 << 0)

/*DYNPD*/
#define DPL_Px(pip) (1 << (pip))

/*FEATURE*/
#define EN_DPL (1 << 2)
#define EN_ACK_PAY (1 << 1)
#define EN_DYN_ACK (1 << 0)

#define get_reg_size(addr) \
  (((addr == 0x0a) || (addr == 0x0b) || (addr == 0x10)) ? 5 : 1)
