/*
 The contents of this file are subject to the Mozilla Public License        
 Version 1.1 (the "License"); you may not use this file except in           
 compliance with the License. You may obtain a copy of the License at       
 http://www.mozilla.org/MPL/                                                
                                                                            
 Software distributed under the License is distributed on an "AS IS"        
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the    
 License for the specific language governing rights and limitations         
 under the License.                                                         
                                                                            
 Alternatively, the contents of this file may be used under the terms       
 of the GNU Lesser General Public license (the  "LGPL License"), in which case the  
 provisions of LGPL License are applicable instead of those                  
 above.                                                                     
                                                                            
 For feedback and questions about my Files and Projects please mail me,     
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com                         
*/
#include <SDL/SDL_mixer.h>
#include "game.h"

Mix_Music *music;

int main(int argc, char **argv)
{
  srand(time(NULL));
  engineSetKeymap("./font/StayPuft.ttf");
  initEngine();
  #ifdef REALGP2X
    Mix_OpenAudio(44100,AUDIO_S16SYS,2,256);
  #else
    Mix_OpenAudio(44100,AUDIO_S16SYS,2,2048);
  #endif
  music = Mix_LoadMUS("./sounds/Cosmic Conundrum.ogg");
  Mix_PlayMusic(music,-1);
  prepare_game_objects(1,6);
  run_game(1,timeMode,0);
  delete_game_objects();
  Mix_CloseAudio();
  quitEngine();
  return 0;
}
