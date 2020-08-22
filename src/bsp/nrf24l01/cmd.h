
#pragma once

#define R_REGISTER 0x00
#define W_REGISTER 0x20
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xa0
#define FLUSH_TX 0xe1
#define FLUSH_RX 0xe2
#define REUSE_TX_PL 0xe3
#define R_RX_PL_WID 0x60
#define W_ACK_PAYLOAD 0xa8
#define W_TX_PAYLOAD_NACK 0xb0
#define NOP 0xff

#define MAX_FIFO_SZ 32

