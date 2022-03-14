#ifndef UTILS_H_
#define UTILS_H_

#define CHOP(pointer, TYPE)                                                   \
  *(TYPE *) pointer;                                                          \
  pointer += sizeof (TYPE)

#define global static

global const char note_name[12][3] = { "C-", "C#", "D-", "D#", "E-", "F-",
                                       "F#", "G-", "G#", "A-", "A#", "B-" };

#endif // UTILS_H_
