#pragma once

#define REQ_CODE "update request"

#define START_CMD "update"
#define START_CMD_ACK "ok"

#define PACKET_MAX_SZ 512

#define START_CODE 0x7e

#pragma pack(1)
typedef struct {
  uint8_t uc_start;
  uint16_t us_index;
  uint16_t us_len;
  uint8_t auc_data[0];
} st_pkt_head_t;

typedef struct {
  uint8_t uc_crc;
} st_pkt_tail_t;

#pragma pack()
