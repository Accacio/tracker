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
print_XM_header (XM_header header)
{
  printf ("id: %.17s\n", header.id); // %Extended Module: %
  printf ("name: %.20s\n", header.module_name);
  printf ("tracker: %.20s\n", header.tracker_name);
  printf ("version: %04x\n", header.version);       // current is 104
  printf ("header size: %d\n", header.header_size); //
  printf ("length: %02x\n", header.song_length);    //
  printf ("restart position: %02x\n",
          header.song_restart_position);                //
  printf ("channels: %02x\n", header.n_channels);       //
  printf ("patterns: %02x\n", header.n_patterns);       //
  printf ("instruments: %02x\n", header.n_instruments); //
  printf ("flags: %02x\n", header.flags);               //
  printf ("tempo: %d\n", header.default_tempo);         //
  printf ("bpm: %d\n", header.bpm);                     //
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
          print_note (cur[0]);
          printf (" %02X %02X %02X %02X | ", cur[1], cur[2], cur[3], cur[4]);
        }
      printf ("\n");
    }
}

typedef struct __attribute__ ((__packed__)) XM_INSTRUMENT_HEADER
{
  uint32_t length;
  uint8_t name[22];
  uint8_t type;
  uint16_t n_samples;
} XM_instrument_header;

void
print_instrument_header (XM_instrument_header header)
{
  printf ("header size: %02X\n", header.length);
  printf ("name: %.20s\n", header.name);
  printf ("samples: %02X\n", header.n_samples);
}

typedef struct __attribute__ ((__packed__)) XM_ENVELOP_POINT
{
  uint16_t frame;
  uint16_t value; // between 0 and 64
} XM_envelope_point;

typedef struct __attribute__ ((__packed__)) XM_EXTENDED_INSTRUMENT_HEADER
{
  uint32_t length;
  uint8_t sample_per_note[96];
  XM_envelope_point volume_envelope_points[12];
  XM_envelope_point panning_envelope_points[12];
  uint8_t n_volume_envelope_points;
  uint8_t n_panning_envelope_points;
  uint8_t volume_sustain_point;
  uint8_t volume_loop_start;
  uint8_t volume_loop_end;
  uint8_t panning_sustain_point;
  uint8_t panning_loop_start;
  uint8_t panning_loop_end;
  uint8_t volume_type;
  uint8_t panning_type;
  uint8_t vibrato_type;
  uint8_t vibrato_sweep;
  uint8_t vibrato_depth;
  uint8_t vibrato_rate;
  uint16_t volume_fadeout;
  uint16_t not_used;
} XM_extended_instrument_header;

void
print_extendend_instrument_header (XM_extended_instrument_header header)
{
  printf("header size: %02X\n", header.length);

  printf("sample per note:\n");
  for (int i = 0; i < 96/12; i++) {
      printf(" \e[1;37;40m%02X\e[0m", header.sample_per_note[12*i+1]);
      printf(" \e[1;37;40m%02X\e[0m", header.sample_per_note[12*i+3]);
      printf("   ");
      printf(" \e[1;37;40m%02X\e[0m", header.sample_per_note[12*i+6]);
      printf(" \e[1;37;40m%02X\e[0m", header.sample_per_note[12*i+8]);
      printf(" \e[1;37;40m%02X\e[0m", header.sample_per_note[12*i+10]);
      printf("   ");
  }
  printf("\n");
  for (int i = 0; i < 96/12; i++) {
    for (int j = 0; j < 12; j++) {
      if(!(j==1|j==3|j==6|j==8|j==10))
      printf("\e[22;30;47m%02X\e[0m ", header.sample_per_note[12*i+j]);
    }
  }
  printf("\n");

  printf ("volume envelope: \n\t");
  for (int i = 0; i < header.n_volume_envelope_points; i++)
    {
      XM_envelope_point point = header.volume_envelope_points[i];
      printf ("(%03d,%03d) ", point.frame, point.value);
    }
  printf ("\n");

  printf ("panning envelope: \n\t");
  for (int i = 0; i < header.n_panning_envelope_points; i++)
    {
      XM_envelope_point point = header.panning_envelope_points[i];
      printf ("(%03d,%03d) ", point.frame, point.value);
    }
  printf ("\n");
  printf ("volume sustain point: %d\n", header.volume_sustain_point);
  printf ("volume loop start: %d\n", header.volume_loop_start);
  printf ("volume loop end: %d\n", header.volume_loop_end);
  printf ("panning sustain point: %d\n", header.panning_sustain_point);
  printf ("panning loop start: %d\n", header.panning_loop_start);
  printf ("panning loop end: %d\n", header.panning_loop_end);
  printf ("volume type: %d\n", header.volume_type);
  printf ("panning type: %d\n", header.panning_type);
  printf ("vibrato type: %d\n", header.vibrato_type);
  printf ("vibrato sweep: %02X\n", header.vibrato_sweep);
  printf ("vibrato depth: %02X\n", header.vibrato_depth);
  printf ("vibrato rate: %02X\n", header.vibrato_rate);
  printf ("volume fadeout: %02X\n", header.volume_fadeout);
}
#endif // XM_H_
