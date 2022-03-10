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

  XM_header xm_header = *(XM_header *) pointer;
  pointer += sizeof (XM_header);
  print_XM_header (xm_header);

  for (int i = 0; i < xm_header.song_length; i++)
    {
      printf ("%02x %02x\n", i, xm_header.patter_order_table[i]); //
      /* printf ("%02x ", xm_header.patter_order_table[i]); // */
    }
  printf ("\n");
  XM_pattern *patterns = malloc (xm_header.n_patterns * sizeof (XM_pattern));
  memset (patterns, 0, xm_header.n_patterns * sizeof (XM_pattern));

  for (int pattern_idx = 0; pattern_idx < xm_header.n_patterns; pattern_idx++)
    {

      XM_pattern *pattern = &patterns[pattern_idx];
      pattern->header = *(XM_pattern_header *) pointer;
      pattern->n_channels = xm_header.n_channels;
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
            XM_pattern_note *cur_note = &pattern->data[i][j];

            uint8_t byte = *pointer;
            pointer++;
            if (byte & 0x80)
              {
                cur_note->note = byte & 0X01 ? *pointer++ : 0;
                cur_note->instrument = byte & 0X02 ? *pointer++ : 0;
                cur_note->volume = byte & 0X04 ? *pointer++ : 0;
                cur_note->effect = byte & 0X08 ? *pointer++ : 0;
                cur_note->effect_parameter = byte & 0X10 ? *pointer++ : 0;
              }
            else
              {
                cur_note->note = byte;
                cur_note->instrument = *pointer++;
                cur_note->volume = *pointer++;
                cur_note->effect = *pointer++;
                cur_note->effect_parameter = *pointer++;
              }
          }
      /* printf ("Pattern %02X\n", pattern_idx); */
      /* print_pattern_header (*pattern); */
      /* print_pattern_data (*pattern); */
      /* printf ("\n"); */
    }

  XM_instrument_header instrument_header = *(XM_instrument_header *) pointer;
  pointer += sizeof (XM_instrument_header);
  print_instrument_header (instrument_header);

  XM_extended_instrument_header extended_instrument_header
      = *(XM_extended_instrument_header *) pointer;
  pointer += sizeof (XM_extended_instrument_header);
  print_extendend_instrument_header (extended_instrument_header);

  XM_sample_header sample_header = *(XM_sample_header *) pointer;
  pointer += sizeof (XM_sample_header);
  print_sample_header (sample_header);

  XM_sample_header sample_header_2 = *(XM_sample_header *) pointer;
  pointer += sizeof (XM_sample_header);
  print_sample_header (sample_header_2);

  XM_sample sample = { 0 };

  if ((sample_header.type & 0X10)) /* if 16bit */
    {
      sample.n_samples = sample_header.length / 2;
      sample.data = (int32_t *) malloc (sample.n_samples * sizeof (int32_t));
      memset (sample.data, 0, sample.n_samples * sizeof (int32_t));

      int16_t old = 0;
      for (int i = 0; i < sample.n_samples; i++)
        {
          old += *((int16_t *) pointer);
          sample.data[i] = old;
          pointer += 2;
        }
    }
  printf ("%02X\n", (uint16_t) (pointer - xm_contents));

  XM_sample sample2 = { 0 };
  if (!(sample_header_2.type & 0X10)) /* if 8bit */
    {
      sample.n_samples = sample_header_2.length ;
      sample.data = (int32_t *) malloc (sample.n_samples * sizeof (int32_t));
      memset (sample.data, 0, sample.n_samples * sizeof (int32_t));

      int8_t old = 0;
      for (int i = 0; i < sample.n_samples; i++)
        {
          old += *((int8_t *) pointer);
          sample.data[i] = old;
          pointer += 1;
        }
    }

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
