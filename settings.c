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
#include "settings.h"
#include <SDL.h>

int settings_stone_quality; //2 perfect, 1 okay, 0 gp2x
int settings_stars_rotating;
int settings_particles;
int settings_volume;

void settings_load()
{
  #ifdef REALGP2X
    settings_stone_quality = 0;
  #else
    settings_stone_quality = 2;  
  #endif
  settings_stars_rotating = 1;
  settings_particles = 1;
  settings_volume = 100;
  SDL_RWops *file=SDL_RWFromFile("./settings.dat","rb");
  if (file == NULL)
    return;
  SDL_RWread(file,&settings_stone_quality,sizeof(int),1);
  SDL_RWread(file,&settings_stars_rotating,sizeof(int),1);
  SDL_RWread(file,&settings_particles,sizeof(int),1);
  SDL_RWread(file,&settings_volume,sizeof(int),1);
  SDL_RWclose(file);
}

void settings_save()
{
  SDL_RWops *file=SDL_RWFromFile("./settings.dat","wb");
  SDL_RWwrite(file,&settings_stone_quality,sizeof(int),1);
  SDL_RWwrite(file,&settings_stars_rotating,sizeof(int),1);
  SDL_RWwrite(file,&settings_particles,sizeof(int),1);
  SDL_RWwrite(file,&settings_volume,sizeof(int),1);
  SDL_RWclose(file);
}

int settings_get_stone_quality()
{
  return settings_stone_quality;
}

int settings_get_stars_rotating()
{
  return settings_stars_rotating;
}

int settings_get_particles()
{
  return settings_particles;
}

int settings_get_volume()
{
  return settings_volume;
}

void settings_set_stone_quality(int value)
{
  settings_stone_quality = value;
}

void settings_set_stars_rotating(int value)
{
  settings_stars_rotating = value;
}

void settings_set_particles(int value)
{
  settings_particles = value;
}

void settings_set_volume(int value)
{
  settings_volume = value;
}
