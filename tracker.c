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

  XM_header *xm = (XM_header *) xm_contents;
  printf ("file size: %dB\n", size_bytes);
  printf ("id: %.17s\n", xm->id); // %Extended Module: %
  printf ("name: %.20s\n", xm->module_name);
  /* printf("%02x\n",xm->not_used1); // 1a */
  printf ("tracker: %.20s\n", xm->tracker_name);
  printf ("version: %02x\n", xm->version);       // current is 104
  printf ("header size: %d\n", xm->header_size); //
  printf ("length: %02x\n", xm->song_length);    //
  printf ("restart position: %02x\n", xm->song_restart_position); //
  printf ("channels: %02x\n", xm->number_channels);               //
  printf ("patterns: %02x\n", xm->number_patterns);               //
  printf ("instruments: %02x\n", xm->number_instruments);         //
  printf ("flags: %02x\n", xm->flags);                            //
  printf ("tempo: %d\n", xm->default_tempo);                      //
  printf ("bpm: %d\n", xm->bpm);                                  //
  printf ("bpm: %x\n", 135);                                      //
  for (int i = 0; i < xm->song_length; i++)
    {
      printf ("%02x %02x\n", i, xm->patter_order_table[i]); //
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
  printf ("tracker\n");

  int device = SDL_OpenAudioDevice (NULL, 0, &wanted, &obtained,
                                    SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

  SDL_PauseAudioDevice (device, 0);
  SDL_Delay (1000);
  SDL_PauseAudioDevice (device, 1);

  SDL_Quit ();
  return 0;
}
