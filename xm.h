#ifndef XM_H_
#define XM_H_

#include <stdint.h>
#include <stdio.h>

#include "utils.h"

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
  uint16_t n_channels;
  uint16_t n_patterns;
  uint16_t n_instruments;
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
  uint16_t size;
} XM_pattern_header;

typedef struct XM_PATTERN
{
  XM_pattern_header header;
  void *data;
} XM_pattern;

enum
{
  PATTERN_NOTE,
  PATTERN_INSTRUMENT,
  PATTERN_VOLUME,
  PATTERN_EFFECT,
  PATTERN_EFFECT_PARAM,
};

global const char note_name[12][3] = { "C-", "C#", "D-", "D#", "E-", "F-",
                                       "F#", "G-", "G#", "A-", "A#", "B-" };

void
print_note (uint8_t note)
{
  if (note > 1 & note < 97)
    {
      --note;
      uint8_t octave = note / 12;
      uint8_t note_in_scale = note % 12;
      printf ("%s%d", note_name[note_in_scale], octave);
    }
  else
    {
      if( note == 97 )
      printf ("___");
      else
        printf("---");
    }
}

#endif // XM_H_
