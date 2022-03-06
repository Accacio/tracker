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
  for (int pattern_idx = 0; pattern_idx < xm_header.n_patterns; pattern_idx++)
    {
      XM_pattern pattern = {};
      pattern.header = *(XM_pattern_header *) pointer;
      pattern.n_channels = xm_header.n_channels;
      pointer += sizeof (XM_pattern_header);

      uint64_t pattern_size = pattern.header.n_rows * pattern.n_channels;
      pattern.data = malloc (pattern_size * sizeof (XM_pattern_note));
      memset (pattern.data, 0, pattern_size * sizeof (XM_pattern_note));

      for (int j = 0; j < pattern.header.n_rows; j++)
        for (int i = 0; i < pattern.n_channels; i++)
          {
            XM_pattern_note *cur_note
                = (XM_pattern_note*) pattern.data + i * sizeof (XM_pattern_note)
                  + j * sizeof (XM_pattern_note) * pattern.n_channels;
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
      /* print_pattern_header (pattern); */
      /* printf ("Pattern %02X\n", pattern_idx); */
      /* print_pattern_data (pattern); */
      /* printf ("\n"); */
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
