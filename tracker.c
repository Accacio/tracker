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
  FILE *xm_file = fopen (filename, "rb");
  if (!xm_file)
    {
      return (1);
    }
  fseek (xm_file, 0, SEEK_END);

  int size_bytes = ftell (xm_file);
  /* printf ("file size: %dB\n", size_bytes); */

  rewind (xm_file);
  uint8_t *xm_contents = (uint8_t *) malloc (size_bytes);
  fread (xm_contents, 1, size_bytes, xm_file);
  uint8_t *pointer = xm_contents;

  XM_song song = { 0 };
  song.header = *(XM_song_header *) pointer;
  pointer += sizeof (XM_song_header);

  song.patterns
      = (XM_pattern *) malloc (song.header.n_patterns * sizeof (XM_pattern));
  memset (song.patterns, 0, song.header.n_patterns * sizeof (XM_pattern));

  for (int pat_idx = 0; pat_idx < song.header.n_patterns; pat_idx++)
    {
      XM_pattern *pattern = &song.patterns[pat_idx];
      pattern->header = *(XM_pattern_header *) pointer;
      pattern->n_channels = song.header.n_channels;
      pointer += sizeof (XM_pattern_header);

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

            uint8_t byte = *pointer;
            pointer++;
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

  if (song.header.n_instruments)
    {
      song.instruments = (XM_instrument *) malloc (song.header.n_instruments
                                                   * sizeof (XM_instrument));
      memset (song.instruments, 0,
              song.header.n_instruments * sizeof (XM_instrument));

      for (int inst_idx = 0; inst_idx < song.header.n_instruments; inst_idx++)
        {
          XM_instrument *instrument = &song.instruments[inst_idx];

          instrument->header = *(XM_instrument_header *) pointer;
          pointer += sizeof (XM_instrument_header);

          if (instrument->header.n_samples)
            {
              instrument->extended_header
                  = *(XM_extended_instrument_header *) pointer;
              pointer += sizeof (XM_extended_instrument_header);

              instrument->samples = (XM_sample *) malloc (
                  instrument->header.n_samples * sizeof (XM_sample));

              for (int sample_idx = 0;
                   sample_idx < instrument->header.n_samples; sample_idx++)
                {
                  XM_sample *sample = &instrument->samples[sample_idx];

                  sample->header = *(XM_sample_header *) pointer;
                  pointer += sizeof (XM_sample_header);
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
                          old += *((int16_t *) pointer);
                          sample->data[i] = old;
                          pointer += 2;
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
                          old += *((int8_t *) pointer);
                          sample->data[sample_sample] = old;
                          pointer += 1;
                        }
                    }
                }
            }
        }
    }
  if ((pointer - xm_contents) == size_bytes)
    printf ("All contents in file were parsed!\n");

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
