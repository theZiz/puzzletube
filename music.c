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
char musicAuthor[256];
Mix_Music *music;
int musicFade;

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

void change_music(char* name,char* author)
{
  musicFade = 0;
  if (strcmp(musicName,name)==0)
    return;
  sprintf(musicName,"%s",name);
  sprintf(musicAuthor,"%s",author);
  char buffer[512];
  sprintf(buffer,"./sounds/%s.ogg",name);
  music = Mix_LoadMUS(buffer);
  Mix_PlayMusic(music,-1);
  Mix_VolumeMusic(settings_get_volume()*128/100);
}

void draw_music()
{
  if (musicFade>5000)
    return;
  char buffer[800];
  int add = 0;
  if (musicFade<1000)
    add = engineGetSurface(SURFACE_KEYMAP)->h*musicFade/5000;
  else
  if (musicFade<4000)
    add = engineGetSurface(SURFACE_KEYMAP)->h/5;
  else
    add = engineGetSurface(SURFACE_KEYMAP)->h*(5000-musicFade)/5000;
  
  sprintf(buffer,"%s:",musicAuthor);
  drawtextMX(engineGetSurface(SURFACE_SURFACE),engineGetWindowX()/2,add-3*(engineGetSurface(SURFACE_KEYMAP)->h>>4),buffer,engineGetSurface(SURFACE_KEYMAP));  
  sprintf(buffer,"\"%s\"",musicName);
  drawtextMX(engineGetSurface(SURFACE_SURFACE),engineGetWindowX()/2,add-2*(engineGetSurface(SURFACE_KEYMAP)->h>>4),buffer,engineGetSurface(SURFACE_KEYMAP));  
}

int volume_wait = 0;

void calc_music(int steps)
{ 
  int i;
  for (i = 0;i<steps;i++)
  {
    volume_wait--;
    if (engineGetInput()->button[BUTTON_VOLPLUS] && volume_wait<=0 && settings_get_volume()<100)
    {
      settings_set_volume(settings_get_volume()+1);
      Mix_VolumeMusic(settings_get_volume()*128/100);
      settings_save();
      volume_wait = 25;
    }
    if (engineGetInput()->button[BUTTON_VOLMINUS] && volume_wait<=0 && settings_get_volume()>0)
    {
      settings_set_volume(settings_get_volume()-1);
      Mix_VolumeMusic(settings_get_volume()*128/100);
      settings_save();
      volume_wait = 25;
    }
  }
  if (musicFade>5000)
    return;
  musicFade += steps;
}

void quit_music()
{
  Mix_CloseAudio();
}

