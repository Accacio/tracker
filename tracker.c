#include <stdio.h>
#include <math.h>

#include "xm.h"
#include "beeper.h"

int
main (int argc, char *argv[])
{
  const char *filename = argv[1];
  FILE *xm_file = fopen (filename, "rb");
  if (!xm_file)
    {
      return (1);
    }
  fseek (xm_file, 0, SEEK_END);

  int size_bytes = ftell (xm_file);
  rewind (xm_file);
  void *xm_contents = malloc (size_bytes);
  fread (xm_contents, 1, size_bytes, xm_file);
  uint8_t *pointer = (uint8_t *) (xm_contents);

  XM_header xm = *(XM_header *) pointer;
  pointer += sizeof (XM_header);
  printf ("file size: %dB\n", size_bytes);
  printf ("id: %.17s\n", xm.id); // %Extended Module: %
  printf ("name: %.20s\n", xm.module_name);
  /* printf("%02x\n",xm.not_used1); // 1a */
  printf ("tracker: %.20s\n", xm.tracker_name);
  printf ("version: %04x\n", xm.version);       // current is 104
  printf ("header size: %d\n", xm.header_size); //
  printf ("length: %02x\n", xm.song_length);    //
  printf ("restart position: %02x\n", xm.song_restart_position); //
  printf ("channels: %02x\n", xm.n_channels);                    //
  printf ("patterns: %02x\n", xm.n_patterns);                    //
  printf ("instruments: %02x\n", xm.n_instruments);              //
  printf ("flags: %02x\n", xm.flags);                            //
  printf ("tempo: %d\n", xm.default_tempo);                      //
  printf ("bpm: %d\n", xm.bpm);                                  //
  for (int i = 0; i < xm.song_length; i++)
    {
      /* printf ("%02x %02x\n", i, xm.patter_order_table[i]); // */
      printf ("%02x ", xm.patter_order_table[i]); //
    }

  XM_pattern pattern = {};
  pattern.header = *(XM_pattern_header *) pointer;
  pointer += sizeof (XM_pattern_header);

  printf ("\n");
  printf ("length: %02x\n", pattern.header.length);
  printf ("packing type: %02x\n", pattern.header.packing_type);
  printf ("rows: %04x\n", pattern.header.n_rows);
  printf ("size: %04x\n", pattern.header.size);
  pattern.data = malloc (pattern.header.n_rows * xm.n_channels * 5);
  memset (pattern.data, 0, pattern.header.n_rows * xm.n_channels * 5);
  for (int j = 0; j < pattern.header.n_rows; j++)
    for (int i = 0; i < xm.n_channels; i++)
      {
        uint8_t *cur = pattern.data + i * 5 + j * 5 * xm.n_channels;
        uint8_t byte = *pointer;
        pointer++;
        if (byte & 0x80)
          {
            cur[PAT_NOTE] = byte & 0X01 ? *pointer++ : 0;
            cur[PAT_INSTRUMENT] = byte & 0X02 ? *pointer++ : 0;
            cur[PAT_VOLUME] = byte & 0X04 ? *pointer++ : 0;
            cur[PAT_FX] = byte & 0X08 ? *pointer++ : 0;
            cur[PAT_FX_PARAM] = byte & 0X10 ? *pointer++ : 0;
          }
        else
          {
            cur[PAT_NOTE] = byte;
            cur[PAT_INSTRUMENT] = *pointer++;
            cur[PAT_VOLUME] = *pointer++;
            cur[PAT_FX] = *pointer++;
            cur[PAT_FX_PARAM] = *pointer++;
          }
      }
  print_pattern (pattern, xm.n_channels);

  /* PatternNote ranges in 1..96 */
  /*   1   = C-0 */
  /*   96  = B-7 */
  /*   97  = Key Off (special 'note') */

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
