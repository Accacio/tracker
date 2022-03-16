#ifndef XM_H_
#define XM_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

typedef struct __attribute__ ((__packed__)) XM_SONG_HEADER
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
} XM_song_header;

typedef struct __attribute__ ((__packed__)) XM_PATTERN_HEADER
{
  uint32_t size;
  uint8_t packing_type;
  uint16_t n_rows;
  uint16_t length;
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
  printf ("size: %02x\n", pattern.header.size);
  printf ("packing type: %02x\n", pattern.header.packing_type);
  printf ("rows: %04x\n", pattern.header.n_rows);
  printf ("length: %04x\n", pattern.header.length);
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
void
print_pattern (XM_pattern pattern)
{
  print_pattern_header (pattern);
  print_pattern_data (pattern);
}

typedef struct __attribute__ ((__packed__)) XM_INSTRUMENT_HEADER
{
  uint32_t size;
  uint8_t name[22];
  uint8_t type;
  uint16_t n_samples;
} XM_instrument_header;

typedef struct __attribute__ ((__packed__)) XM_ENVELOP_POINT
{
  uint16_t frame;
  uint16_t value; // between 0 and 64
} XM_envelope_point;

typedef struct __attribute__ ((__packed__)) XM_EXTENDED_INSTRUMENT_HEADER
{
  uint32_t size;
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
  uint16_t not_used[11];
} XM_extended_instrument_header;

typedef struct __attribute__ ((__packed__)) XM_SAMPLE_HEADER
{
  uint32_t length;
  uint32_t loop_start;
  uint32_t loop_length;
  uint8_t volume;
  uint8_t fine_tune;
  uint8_t type;
  uint8_t panning;
  uint8_t relative_note;
  uint8_t not_used;
  uint8_t name[22];
} XM_sample_header;

typedef struct XM_SAMPLE
{
  XM_sample_header header;
  uint32_t n_samples;
  int32_t *data;
} XM_sample;

void
print_sample_header (XM_sample sample)
{
  printf ("length: %04X\n", sample.header.length);
  printf ("loop start: %04X\n", sample.header.loop_start);
  printf ("loop length: %04X\n", sample.header.loop_length);
  printf ("volume: %d\n", sample.header.volume);
  printf ("fine tune: %d\n", sample.header.fine_tune);
  printf ("type: %02X\n", sample.header.type);
  printf ("panning: %02X\n", sample.header.panning);
  printf ("name: %.22s\n", sample.header.name);
}

typedef struct XM_INSTRUMENT
{
  XM_instrument_header header;
  XM_extended_instrument_header extended_header;
  XM_sample *samples;
} XM_instrument;

void
print_instrument_header (XM_instrument instrument)
{
  printf ("header size: %04X\n", instrument.header.size);
  printf ("type: %02X\n", instrument.header.type);
  printf ("name: %.22s\n", instrument.header.name);
  printf ("samples: %02X\n", instrument.header.n_samples);
}

void
print_extendend_instrument_header (XM_instrument instrument)
{
  printf ("header size: %02X\n", instrument.extended_header.size);

  printf ("sample per note:\n");
  for (int i = 0; i < 96 / 12; i++)
    {
      printf (" \e[1;37;40m%02X\e[0m",
              instrument.extended_header.sample_per_note[12 * i + 1]);
      printf (" \e[1;37;40m%02X\e[0m",
              instrument.extended_header.sample_per_note[12 * i + 3]);
      printf ("   ");
      printf (" \e[1;37;40m%02X\e[0m",
              instrument.extended_header.sample_per_note[12 * i + 6]);
      printf (" \e[1;37;40m%02X\e[0m",
              instrument.extended_header.sample_per_note[12 * i + 8]);
      printf (" \e[1;37;40m%02X\e[0m",
              instrument.extended_header.sample_per_note[12 * i + 10]);
      printf ("   ");
    }
  printf ("\n");
  for (int i = 0; i < 96 / 12; i++)
    {
      for (int j = 0; j < 12; j++)
        {
          if (!(j == 1 | j == 3 | j == 6 | j == 8 | j == 10))
            printf ("\e[1;30;47m%02X\e[0m ",
                    instrument.extended_header.sample_per_note[12 * i + j]);
        }
    }
  printf ("\n");

  printf ("volume envelope: \n\t");
  for (int i = 0; i < instrument.extended_header.n_volume_envelope_points; i++)
    {
      XM_envelope_point point
          = instrument.extended_header.volume_envelope_points[i];
      printf ("(%03d,%03d) ", point.frame, point.value);
    }
  printf ("\n");

  printf ("panning envelope: \n\t");
  for (int i = 0; i < instrument.extended_header.n_panning_envelope_points;
       i++)
    {
      XM_envelope_point point
          = instrument.extended_header.panning_envelope_points[i];
      printf ("(%03d,%03d) ", point.frame, point.value);
    }
  printf ("\n");
  printf ("volume sustain point: %d\n",
          instrument.extended_header.volume_sustain_point);
  printf ("volume loop start: %d\n",
          instrument.extended_header.volume_loop_start);
  printf ("volume loop end: %d\n", instrument.extended_header.volume_loop_end);
  printf ("panning sustain point: %d\n",
          instrument.extended_header.panning_sustain_point);
  printf ("panning loop start: %d\n",
          instrument.extended_header.panning_loop_start);
  printf ("panning loop end: %d\n",
          instrument.extended_header.panning_loop_end);
  printf ("volume type: %d\n", instrument.extended_header.volume_type);
  printf ("panning type: %d\n", instrument.extended_header.panning_type);
  printf ("vibrato type: %d\n", instrument.extended_header.vibrato_type);
  printf ("vibrato sweep: %02X\n", instrument.extended_header.vibrato_sweep);
  printf ("vibrato depth: %02X\n", instrument.extended_header.vibrato_depth);
  printf ("vibrato rate: %02X\n", instrument.extended_header.vibrato_rate);
  printf ("volume fadeout: %02X\n", instrument.extended_header.volume_fadeout);
}

void
print_instrument (XM_instrument instrument)
{
  print_instrument_header (instrument);
  print_extendend_instrument_header (instrument);
  for (int sample_idx = 0; sample_idx < instrument.header.n_samples;
       sample_idx++)
    {
      printf ("=== Sample %02X ===\n", sample_idx);
      XM_sample sample = instrument.samples[sample_idx];
      print_sample_header (sample);
    }
}

typedef struct XM_SONG
{
  XM_song_header header;
  XM_pattern *patterns;
  XM_instrument *instruments;
} XM_song;

void
print_song_header (XM_song song)
{
  printf ("id: %.17s\n", song.header.id); // %Extended Module: %
  printf ("name: %.20s\n", song.header.module_name);
  printf ("tracker: %.20s\n", song.header.tracker_name);
  printf ("version: %04x\n", song.header.version);       // current is 104
  printf ("header size: %d\n", song.header.header_size); //
  printf ("length: %02x\n", song.header.song_length);    //
  printf ("restart position: %02x\n",
          song.header.song_restart_position);                //
  printf ("channels: %02x\n", song.header.n_channels);       //
  printf ("patterns: %02x\n", song.header.n_patterns);       //
  printf ("instruments: %02x\n", song.header.n_instruments); //
  printf ("flags: %02x\n", song.header.flags);               //
  printf ("tempo: %d\n", song.header.default_tempo);         //
  printf ("bpm: %d\n", song.header.bpm);                     //
  for (int i = 0; i < song.header.song_length; i++)
    {
      printf ("%02x %02x\n", i, song.header.patter_order_table[i]); //
      /* printf ("%02x ", xm_header.patter_order_table[i]); // */
    }
  printf ("\n");
}

uint8_t
read_song_from_file (const char *filename, XM_song *song)
{
  FILE *xm_file = fopen (filename, "rb");
  if (!xm_file)
    {
      return 0;
    }
  fseek (xm_file, 0, SEEK_END);

  int size_bytes = ftell (xm_file);
  /* printf ("file size: %dB\n", size_bytes); */

  rewind (xm_file);
  uint8_t *xm_contents = (uint8_t *) malloc (size_bytes);
  fread (xm_contents, 1, size_bytes, xm_file);
  fclose (xm_file);

  uint8_t *pointer = xm_contents;

  song->header = CHOP (pointer, XM_song_header);

  song->patterns
      = (XM_pattern *) malloc (song->header.n_patterns * sizeof (XM_pattern));
  memset (song->patterns, 0, song->header.n_patterns * sizeof (XM_pattern));

  for (int pat_idx = 0; pat_idx < song->header.n_patterns; pat_idx++)
    {
      XM_pattern *pattern = &song->patterns[pat_idx];
      pattern->header = CHOP (pointer, XM_pattern_header);
      pattern->n_channels = song->header.n_channels;

      uint64_t pattern_size = pattern->header.n_rows * pattern->n_channels;
      pattern->data = (XM_pattern_note **) malloc (
          pattern->n_channels * sizeof (XM_pattern_note *));
      for (int i = 0; i < pattern->n_channels; i++)
        {
          pattern->data[i] = (XM_pattern_note *) malloc (
              pattern->header.n_rows * sizeof (XM_pattern_note));
          memset (pattern->data[i], 0,
                  pattern->header.n_rows * sizeof (XM_pattern_note));
        }

      for (int j = 0; j < pattern->header.n_rows; j++)
        for (int i = 0; i < pattern->n_channels; i++)
          {
            XM_pattern_note *note = &pattern->data[i][j];

            uint8_t byte = CHOP (pointer, uint8_t);

            if (byte & 0x80)
              {
                note->note = byte & 0X01 ? *pointer++ : 0;
                note->instrument = byte & 0X02 ? *pointer++ : 0;
                note->volume = byte & 0X04 ? *pointer++ : 0;
                note->effect = byte & 0X08 ? *pointer++ : 0;
                note->effect_parameter = byte & 0X10 ? *pointer++ : 0;
              }
            else
              {
                note->note = byte;
                note->instrument = *pointer++;
                note->volume = *pointer++;
                note->effect = *pointer++;
                note->effect_parameter = *pointer++;
              }
          }
    }

  if (song->header.n_instruments)
    {
      song->instruments = (XM_instrument *) malloc (song->header.n_instruments
                                                    * sizeof (XM_instrument));
      memset (song->instruments, 0,
              song->header.n_instruments * sizeof (XM_instrument));

      for (int inst_idx = 0; inst_idx < song->header.n_instruments; inst_idx++)
        {
          XM_instrument *instrument = &song->instruments[inst_idx];

          instrument->header = CHOP (pointer, XM_instrument_header);

          if (instrument->header.n_samples)
            {
              instrument->extended_header
                  = CHOP (pointer, XM_extended_instrument_header);

              instrument->samples = (XM_sample *) malloc (
                  instrument->header.n_samples * sizeof (XM_sample));

              for (int sample_idx = 0;
                   sample_idx < instrument->header.n_samples; sample_idx++)
                {
                  XM_sample *sample = &instrument->samples[sample_idx];

                  sample->header = CHOP (pointer, XM_sample_header);
                }

              for (int sample_idx = 0;
                   sample_idx < instrument->header.n_samples; sample_idx++)
                {
                  XM_sample *sample = &instrument->samples[sample_idx];
                  if ((sample->header.type & 0X10)) /* if 16bit */
                    {
                      sample->n_samples = sample->header.length / 2;
                      sample->data = (int32_t *) malloc (sample->n_samples
                                                         * sizeof (int32_t));
                      memset (sample->data, 0,
                              sample->n_samples * sizeof (int32_t));

                      int16_t old = 0;
                      for (int i = 0; i < sample->n_samples; i++)
                        {
                          old += CHOP (pointer, int16_t);
                          sample->data[i] = old;
                        }
                    }
                  else
                    {
                      sample->n_samples = sample->header.length;
                      sample->data = (int32_t *) malloc (sample->n_samples
                                                         * sizeof (int32_t));
                      memset (sample->data, 0,
                              sample->n_samples * sizeof (int32_t));

                      int8_t old = 0;
                      for (int sample_sample = 0;
                           sample_sample < sample->n_samples; sample_sample++)
                        {
                          old += CHOP (pointer, int8_t);
                          sample->data[sample_sample] = old;
                        }
                    }
                }
            }
          else
            {
              pointer
                  += instrument->header.size - sizeof (XM_instrument_header);
            }
        }
    }
  free (xm_contents);

  if ((pointer - xm_contents) == size_bytes)
    printf ("All contents in file were parsed!\n");
  return 1;
}

void
print_song (XM_song song)
{
  printf ("=== SONG ===\n");
  print_song_header (song);
  printf ("\n");
  for (int pat_idx = 0; pat_idx < song.header.n_patterns; pat_idx++)
    {
      printf ("=== Pattern %02X ===\n", pat_idx);
      XM_pattern pattern = song.patterns[pat_idx];
      print_pattern (pattern);
    }
  for (int inst_idx = 0; inst_idx < song.header.n_instruments; inst_idx++)
    {
      printf ("=== Instrument %02X ===\n", inst_idx);
      XM_instrument instrument = song.instruments[inst_idx];
      print_instrument (instrument);
    }
}

#endif // XM_H_
