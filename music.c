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

#include "music.h"

char musicName[256];
Mix_Music *music;

void init_music()
{
  #ifdef REALGP2X
    Mix_OpenAudio(44100,AUDIO_S16SYS,2,256);
  #else
    Mix_OpenAudio(44100,AUDIO_S16SYS,2,2048);
  #endif
  musicName[0]=0;
  music = NULL; 
}

void change_music(char* name)
{
  if (strcmp(musicName,name)==0)
    return;
  sprintf(musicName,"%s",name);
  char buffer[512];
  sprintf(buffer,"./sounds/%s.ogg",name);
  music = Mix_LoadMUS(buffer);
  Mix_PlayMusic(music,-1);  
}

void quit_music()
{
  Mix_CloseAudio();
}

