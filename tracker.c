#include <stdio.h>
#include <math.h>

#include "xm.h"
#include "beeper.h"

int
main (int argc, char *argv[])
{
  if (argc < 2)
    {
      printf ("No input file given\n");
      return (1);
    }
  const char *filename = argv[1];
  const char *output_filename = "output.xm";
  XM_song song = { 0 };

  read_song_from_file (filename, &song);
  /* print_song (song); */
  /* print_song_header (song); */
  /* return 0; */
  FILE *xm_file = fopen (output_filename, "wb");
  fwrite (&song.header, sizeof (XM_song_header), 1, xm_file);
  for (int pat_idx = 0; pat_idx < song.header.n_patterns; pat_idx++)
    {
      XM_pattern *pattern = &song.patterns[pat_idx];
      fwrite (&pattern->header.size, sizeof (uint32_t), 1, xm_file);
      fwrite (&pattern->header.packing_type, sizeof (uint8_t), 1, xm_file);
      fwrite (&pattern->header.n_rows, sizeof (uint16_t), 1, xm_file);

      /* TODO(accacio): precalculate compressed pattern data size */
      /* uint16_t total_size = 5 * pattern->header.n_rows *
       * pattern->n_channels; */
      /* fwrite (&total_size, sizeof (uint16_t), 1, xm_file); */
      uint8_t *compact_pattern = (uint8_t *) malloc (5 * pattern->n_channels
                                                     * pattern->header.n_rows);
      uint8_t *pointer = compact_pattern;

      for (int j = 0; j < pattern->header.n_rows; j++)
        for (int i = 0; i < pattern->n_channels; i++)
          {
            XM_pattern_note note = pattern->data[i][j];

            uint8_t mask = 0x80;
            mask |= note.note ? 0x01 : 0;
            mask |= note.instrument ? 0x02 : 0;
            mask |= note.volume ? 0x04 : 0;
            mask |= note.effect ? 0x08 : 0;
            mask |= note.effect_parameter ? 0x10 : 0;

            if (mask == 0x9F || mask == 0x8F)
              {
                *pointer++ = note.note;
                *pointer++ = note.instrument;
                *pointer++ = note.volume;
                *pointer++ = note.effect;
                *pointer++ = note.effect_parameter;
              }
            else
              {

                *pointer++ = mask;

                if (mask & 0x01)
                  *pointer++ = note.note;
                if (mask & 0x02)
                  *pointer++ = note.instrument;
                if (mask & 0x04)
                  *pointer++ = note.volume;
                if (mask & 0x08)
                  *pointer++ = note.effect;
                if (mask & 0x10)
                  *pointer++ = note.effect_parameter;
              }
          }
      pattern->header.length = (uint16_t) (pointer - compact_pattern);
      fwrite (&pattern->header.length, sizeof (uint16_t), 1, xm_file);

      fwrite (compact_pattern, pattern->header.length, 1, xm_file);
      free (compact_pattern);
    }
  for (int inst_idx = 0; inst_idx < song.header.n_instruments; inst_idx++)
    {
      XM_instrument *instrument = &song.instruments[inst_idx];

      /* fwrite (&instrument->header, sizeof (XM_instrument_header), 1,
       * xm_file); */
      fwrite (&instrument->header.size, sizeof (uint32_t), 1, xm_file);
      fwrite (&instrument->header.name, 22 * sizeof (uint8_t), 1, xm_file);
      fwrite (&instrument->header.type, sizeof (uint8_t), 1, xm_file);
      fwrite (&instrument->header.n_samples, sizeof (uint16_t), 1, xm_file);

      printf ("=== Instrument %02X ===\n", inst_idx);
      print_instrument_header (*instrument);
      if (instrument->header.n_samples > 0)
        {
          fwrite (&instrument->extended_header,
                  sizeof (XM_extended_instrument_header), 1, xm_file);

          for (int sample_idx = 0; sample_idx < instrument->header.n_samples;
               sample_idx++)
            {
              XM_sample *sample = &instrument->samples[sample_idx];

              fwrite (&sample->header, sizeof (XM_sample_header), 1, xm_file);
            }

          for (int sample_idx = 0; sample_idx < instrument->header.n_samples;
               sample_idx++)
            {
              XM_sample *sample = &instrument->samples[sample_idx];

              if ((sample->header.type & 0X10)) /* if 16bit */
                {

                  int16_t old = sample->data[0];
                  fwrite (&old, sizeof (int16_t), 1, xm_file);
                  for (int i = 1; i < sample->n_samples; i++)
                    {
                      old = sample->data[i] - sample->data[i - 1];
                      fwrite (&old, sizeof (int16_t), 1, xm_file);
                    }
                }
              else
                {

                  int8_t old = sample->data[0];
                  fwrite (&old, sizeof (int8_t), 1, xm_file);
                  for (int i = 1; i < sample->n_samples; i++)
                    {
                      old = sample->data[i] - sample->data[i - 1];
                      fwrite (&old, sizeof (int8_t), 1, xm_file);
                    }
                }
            }
        }
      else
        {
          int32_t sample_header_size = sizeof (XM_sample_header);
          fwrite (&sample_header_size, sizeof (int32_t), 1, xm_file);
        }
    }
  fclose (xm_file);

  SDL_InitSubSystem (SDL_INIT_AUDIO);
  SDL_AudioSpec wanted, obtained;

  SDL_zero (wanted);
  wanted.freq = 44100;
  wanted.format = AUDIO_S16SYS;
  wanted.channels = 2;
  wanted.samples = 2048 / 2;
  wanted.callback = fill_audio_buffer;
  /* wanted.callback = NULL; */
  wanted.userdata = &wanted;

  /* int device = SDL_OpenAudioDevice (NULL, 0, &wanted, &obtained, */
  /*                                   SDL_AUDIO_ALLOW_FREQUENCY_CHANGE); */

  /* SDL_PauseAudioDevice (device, 0); */
  /* SDL_Delay (1000); */
  /* SDL_PauseAudioDevice (device, 1); */

  SDL_Quit ();
  return 0;
}
