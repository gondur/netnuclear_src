#include "SDL_mixer.h"
#include "sfx.h"
#include "misc.h"

Mix_Chunk *samples[NUM_SAMPLES];
const struct {
   char *filename;
   int volume;
} sfx_files[] = {
   { "sfx/tele1.wav",    50 },
   { "sfx/tele2.wav",    50 },
   { "sfx/tele3.wav",    50 },
   { "sfx/tele4.wav",    50 },
   { "sfx/tele5.wav",    50 },
   { "sfx/telestop.wav", 50 },
   { "sfx/door.wav",     100 },
   { "sfx/chat.wav",     30 },
   { "sfx/build.wav",    80 },
   { "sfx/radio.wav",    30 },
   { "sfx/fanfare.wav",  80 },
   { "sfx/missile.wav",  100 },
   { "sfx/engine.wav",   60 },
   { "sfx/engine2.wav",  60 },
   { "sfx/boom.wav",     100 },
   { "sfx/dish.wav",     50 },
   { "sfx/feet.wav",     60 },
   { "", 0 }
};

void init_sound() {
   if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
      do_error(Mix_GetError());

   Mix_AllocateChannels(NUM_CHANNELS);
}

void load_samples() {
   int i;

   for (i = 0; i < NUM_SAMPLES; i++) {
      if (!file_exists(sfx_files[i].filename))
         do_error("Couldn't find wave file \"%s\"", sfx_files[i].filename);
      samples[i] = Mix_LoadWAV(sfx_files[i].filename);
      if (samples[i] == NULL)
         do_error("Failed loading wave file \"%s\" (\"%s\")", sfx_files[i].filename, Mix_GetError());

   /* set volume */
      Mix_VolumeChunk(samples[i], MIX_MAX_VOLUME * ((float)sfx_files[i].volume / 100));
   }
}

void play_sample(int num) {
   int loop = 0;

   stop_samples();

/* looping? */
   switch (num) {
      case WAV_BUILD:
      case WAV_RADIO:
      case WAV_ENGINE:
      case WAV_ENGINE2:
      case WAV_DISH:
      case WAV_FEET:
         loop = -1;
         break;
   }

   if (Mix_PlayChannel(-1, samples[num], loop))
      do_error(Mix_GetError());
}

void stop_samples() {
   int i;

   for (i = 0; i < NUM_CHANNELS; i++) {
      if (Mix_Playing(i))
         Mix_HaltChannel(i);
   }
}
