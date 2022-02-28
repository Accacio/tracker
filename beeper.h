#ifndef BEEPER_H_
#define BEEPER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#define PI 3.141592653589793f

#define global static
global int v=0;

void
fill_audio_buffer (void *user_data, Uint8 *_stream, int length)
{
    SDL_AudioSpec audio_spec = *(SDL_AudioSpec*) user_data;
    int i = 0;
    while(i<length/2)
    {
      v += 1;
      Sint16 *stream = (Sint16*) _stream;
      stream[i] = sin (v * 440* PI/audio_spec.freq) * 5000;
      const int sample_size = sizeof (int16_t) * 1;
      i++;
    }
}


#endif // BEEPER_H_
