#include <stdio.h>
#include <math.h>

#include "beeper.h"

int
main (int argc, char *argv[])
{
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
  SDL_Delay (3000);
  SDL_PauseAudioDevice (device, 1);

  SDL_Quit ();
  return 0;
}
