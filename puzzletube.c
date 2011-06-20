#include <SDL/SDL_mixer.h>
#include "game.h"

Mix_Music *music;

int main(int argc, char **argv)
{
  srand(time(NULL));
  initEngine();
  #ifdef REALGP2X
    Mix_OpenAudio(44100,AUDIO_S16SYS,2,256);
  #else
    Mix_OpenAudio(44100,AUDIO_S16SYS,2,2048);
  #endif
  music = Mix_LoadMUS("./sounds/Puzzle_game_tune_2.ogg");
  Mix_PlayMusic(music,-1);
  prepare_game_objects(1,6);
  run_game(1,timeMode,0);
  delete_game_objects();
  Mix_CloseAudio();
  quitEngine();
  return 0;
}
