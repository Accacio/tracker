#ifndef XM_H_
#define XM_H_

#include <stdint.h>

typedef struct __attribute__ ((__packed__)) XM_HEADER
{
  uint8_t id[17];
  uint8_t module_name[20];
  uint8_t not_used1;
  uint8_t tracker_name[20];
  uint16_t version;
  uint32_t header_size;
  uint16_t song_length;
  uint16_t song_restart_position;
  uint16_t number_channels;
  uint16_t number_patterns;
  uint16_t number_instruments;
  uint16_t flags;
  uint16_t default_tempo;
  uint16_t bpm;
  uint8_t patter_order_table[256];
} XM_header;

typedef struct __attribute__ ((__packed__)) XM_PATTERN_HEADER
{
  uint32_t length;
  uint8_t packing_type;
  uint16_t n_rows;
  uint8_t size;
} XM_pattern_header;

typedef struct XM_PATTERN
{
  XM_pattern_header header;
  void *data;
} XM_pattern;

#endif // XM_H_
