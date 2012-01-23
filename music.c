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
#include "settings.h"
char musicName[256];
char musicAuthor[256];
Mix_Music *music;
int musicFade;
Mix_Chunk *move_chunk,*rotating_chunk,*switch_chunk;
Mix_Chunk *explosion_chunk[4];
int rotating_channel;
int move_channel;
char audio_failed;

void init_music()
{
  musicName[0]=0;
  music = NULL; 
  printf("Try to open Audio...\n");
  if (!
  #ifdef REALGP2X
    Mix_OpenAudio(44100,AUDIO_S16SYS,2,256)
  #else
    Mix_OpenAudio(44100,AUDIO_S16SYS,2,2048)
  #endif
  )
  {
    printf("Success\n");
    audio_failed = 0;
  }
  else
  {
    printf("Failed\n");
    audio_failed = 1;
    move_chunk = NULL;
    rotating_chunk = NULL;
    explosion_chunk[0] = NULL;
    explosion_chunk[1] = NULL;
    explosion_chunk[2] = NULL;
    explosion_chunk[3] = NULL;
    switch_chunk = NULL;
    return;
  }
  move_chunk = Mix_LoadWAV("./sounds/Vertical Movement.ogg");
  if (!move_chunk)
    printf("Failed load \"./sounds/Vertical Movement.ogg\"\n  Error: \"%s\"\n", Mix_GetError());
  move_channel = -1;
  rotating_chunk = Mix_LoadWAV("./sounds/Rotating.ogg");
  if (!rotating_chunk)
    printf("Failed load \"./sounds/Rotating.ogg\"\n  Error: \"%s\"\n", Mix_GetError());
  rotating_channel = -1;
  explosion_chunk[0] = Mix_LoadWAV("./sounds/Explosion1.ogg");
  if (!explosion_chunk[0])
    printf("Failed load \"./sounds/Explosion1.ogg\"\n  Error: \"%s\"\n", Mix_GetError());
  explosion_chunk[1] = Mix_LoadWAV("./sounds/Explosion2.ogg");
  if (!explosion_chunk[1])
    printf("Failed load \"./sounds/Explosion1.ogg\"\n  Error: \"%s\"\n", Mix_GetError());
  explosion_chunk[2] = Mix_LoadWAV("./sounds/Explosion3.ogg");
  if (!explosion_chunk[2])
    printf("Failed load \"./sounds/Explosion1.ogg\"\n  Error: \"%s\"\n", Mix_GetError());
  explosion_chunk[3] = Mix_LoadWAV("./sounds/Explosion4.ogg");
  if (!explosion_chunk[3])
    printf("Failed load \"./sounds/Explosion1.ogg\"\n  Error: \"%s\"\n", Mix_GetError());
  switch_chunk = Mix_LoadWAV("./sounds/Tile Switch.ogg");
  if (!switch_chunk)
    printf("Failed load \"./sounds/Tile Switch.ogg\"\n  Error: \"%s\"\n", Mix_GetError());
}

void move_sound_on()
{
  if (!move_chunk)
    return;
  if (move_channel < 0)
    move_channel = Mix_PlayChannel(-1,move_chunk,-1);
  else
    Mix_Resume(move_channel);
}

void move_sound_off()
{
  if (move_channel >= 0)
    Mix_Pause(move_channel);
}

void rotating_sound_on()
{
  if (!rotating_chunk)
    return;
  if (rotating_channel < 0)
    rotating_channel = Mix_PlayChannel(-1,rotating_chunk,-1);
  else
    Mix_Resume(rotating_channel);
}

void rotating_sound_off()
{
  if (rotating_channel >= 0)
    Mix_Pause(rotating_channel);
}

void set_volume(int volume)
{
  volume = volume * 128 / 100;
  if (audio_failed)
    return;
  Mix_HaltChannel(-1);
  rotating_channel = -1;
  move_channel = -1;
  Mix_VolumeMusic(volume);
  if (rotating_chunk)
    Mix_VolumeChunk(rotating_chunk,volume);
  if (explosion_chunk[0])    
    Mix_VolumeChunk(explosion_chunk[0],volume);
  if (explosion_chunk[1])    
    Mix_VolumeChunk(explosion_chunk[1],volume);
  if (explosion_chunk[2])    
    Mix_VolumeChunk(explosion_chunk[2],volume);
  if (explosion_chunk[3])    
    Mix_VolumeChunk(explosion_chunk[3],volume);
  if (switch_chunk)    
    Mix_VolumeChunk(switch_chunk,volume);
  if (move_chunk)    
    Mix_VolumeChunk(move_chunk,volume);
}

void play_explosion()
{
  int number = rand()%4;
  if (!explosion_chunk[number])
    return;
  Mix_PlayChannel(-1,explosion_chunk[number],0);
}

void play_switch()
{
  if (!switch_chunk)
    return;
  Mix_PlayChannel(-1,switch_chunk,0);
}


void change_music(char* name,char* author)
{
  set_volume(settings_get_volume());
  musicFade = 0;
  if (strcmp(musicName,name)==0)
    return;
  sprintf(musicName,"%s",name);
  sprintf(musicAuthor,"%s",author);
  char buffer[512];
  sprintf(buffer,"./sounds/%s.ogg",name);
  if (audio_failed)
    return;
  if (music)
    Mix_FreeMusic(music);
  music = Mix_LoadMUS(buffer);
  Mix_PlayMusic(music,-1);
  set_volume(settings_get_volume());
}

void draw_music()
{
  spFontPointer small_font = settings_get_small_font();
  int engineWindowX=spGetWindowSurface()->w;
  int engineWindowY=spGetWindowSurface()->h;
  if (musicFade>5000)
    return;
  char buffer[800];
  int add = 0;
  if (musicFade<1000)
    add = small_font->maxheight*musicFade*22/10000;
  else
  if (musicFade<4000)
    add = small_font->maxheight*22/10; //2.2
  else
    add = small_font->maxheight*(5000-musicFade)*22/10000;
  
  sprintf(buffer,"%s:",musicAuthor);
  spFontDrawMiddle(engineWindowX/2,add-2*(small_font->maxheight),-1,buffer,small_font);
  sprintf(buffer,"\"%s\"",musicName);
  spFontDrawMiddle(engineWindowX/2,add-1*(small_font->maxheight),-1,buffer,small_font);
}

int volume_wait = 0;

void calc_music(int steps)
{ 
  int i;
  for (i = 0;i<steps;i++)
  {
    volume_wait--;
    if (spGetInput()->button[SP_BUTTON_VOLPLUS] && volume_wait<=0 && settings_get_volume()<100)
    {
      settings_set_volume(settings_get_volume()+1);
      set_volume(settings_get_volume());
      settings_save();
      volume_wait = 25;
    }
    if (spGetInput()->button[SP_BUTTON_VOLMINUS] && volume_wait<=0 && settings_get_volume()>0)
    {
      settings_set_volume(settings_get_volume()-1);
      set_volume(settings_get_volume());
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
  if (audio_failed)
    return;
  Mix_FreeChunk(move_chunk);
  Mix_FreeChunk(rotating_chunk);
  Mix_FreeChunk(explosion_chunk[0]);
  Mix_FreeChunk(explosion_chunk[1]);
  Mix_FreeChunk(explosion_chunk[2]);
  Mix_FreeChunk(explosion_chunk[3]);
  Mix_FreeChunk(switch_chunk);
  if (music)
    Mix_FreeMusic(music);
  Mix_CloseAudio();
}

