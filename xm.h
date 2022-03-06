#ifndef XM_H_
#define XM_H_

#include <stdint.h>
#include <stdio.h>

#include "utils.h"

typedef struct __attribute__ ((__packed__)) XM_HEADER
{
  uint8_t id[17];
  uint8_t module_name[20];
  uint8_t not_used;
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

void
print_XM_header (XM_header xm_header)
{
  printf ("id: %.17s\n", xm_header.id); // %Extended Module: %
  printf ("name: %.20s\n", xm_header.module_name);
  printf ("tracker: %.20s\n", xm_header.tracker_name);
  printf ("version: %04x\n", xm_header.version);       // current is 104
  printf ("header size: %d\n", xm_header.header_size); //
  printf ("length: %02x\n", xm_header.song_length);    //
  printf ("restart position: %02x\n", xm_header.song_restart_position); //
  printf ("channels: %02x\n", xm_header.n_channels);                    //
  printf ("patterns: %02x\n", xm_header.n_patterns);                    //
  printf ("instruments: %02x\n", xm_header.n_instruments);              //
  printf ("flags: %02x\n", xm_header.flags);                            //
  printf ("tempo: %d\n", xm_header.default_tempo);                      //
  printf ("bpm: %d\n", xm_header.bpm);                                  //
}

typedef struct __attribute__ ((__packed__)) XM_PATTERN_HEADER
{
  uint32_t length;
  uint8_t packing_type;
  uint16_t n_rows;
  uint16_t size;
} XM_pattern_header;

typedef struct XM_PATTERN_NOTE
{
  uint8_t note;
  uint8_t instrument;
  uint8_t volume;
  uint8_t effect;
  uint8_t effect_parameter;
} XM_pattern_note;

// NOTE(accacio): Maybe it is better to structure as double array, in
// case we need to add/remove channels and rows
typedef struct XM_PATTERN
{
  XM_pattern_header header;
  uint16_t n_channels;
  XM_pattern_note **data;
} XM_pattern;

enum
{
  PAT_NOTE,
  PAT_INSTRUMENT,
  PAT_VOLUME,
  PAT_FX,
  PAT_FX_PARAM,
};

global const char note_name[12][3] = { "C-", "C#", "D-", "D#", "E-", "F-",
                                       "F#", "G-", "G#", "A-", "A#", "B-" };

/* PatternNote ranges in 1..96 */
/*   1   = C-0 */
/*   96  = B-7 */
/*   97  = Key Off (special 'note') */

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
      if (note == 97)
        printf ("███");
      else
        printf ("---");
    }
}

void
print_pattern_header (XM_pattern pattern)
{
  printf ("length: %02x\n", pattern.header.length);
  printf ("packing type: %02x\n", pattern.header.packing_type);
  printf ("rows: %04x\n", pattern.header.n_rows);
  printf ("size: %04x\n", pattern.header.size);
}
void
print_pattern_data (XM_pattern pattern)
{
  for (int j = 0; j < pattern.header.n_rows; j++)
    {
      printf ("%02X |", j);
      for (int i = 0; i < pattern.n_channels; i++)
        {
          uint8_t *cur = (uint8_t *) &pattern.data[i][j];
          print_note (cur[PAT_NOTE]);
          printf (" %02X %02X %02X %02X | ", cur[1], cur[2], cur[3], cur[4]);
        }
      printf ("\n");
    }
}

#endif // XM_H_
