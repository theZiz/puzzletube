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
#include "../sparrow3d/sparrow3d.h"
#include <SDL.h>

int settings_stone_quality; //2 perfect, 1 okay, 0 gp2x
int settings_stars_rotating;
int settings_particles;
int settings_alpha_blending;
int settings_font_quality;
int settings_volume;
int settings_color;
int settings_difficult;
int settings_mode;
int highscore_choice;

spFontPointer settings_font;
spFontPointer settings_small_font;
spFontPointer settings_middle_font;

int highscore[2][6][10][6];
char highscore_name[2][6][10][6][3];

char get_highscore_name(int i,int j,int k,int l,int m)
{
  return highscore_name[i][j][k][l][m];
}

int get_highscore(int i,int j,int k,int l)
{
  return highscore[i][j][k][l];
}

void insert_highscore(int i,int j,int k,char* name,int points)
{
  if (highscore[i][j][k][5] >= points)
    return;
  highscore_name[i][j][k][5][0]=name[0];
  highscore_name[i][j][k][5][1]=name[1];
  highscore_name[i][j][k][5][2]=name[2];
  highscore[i][j][k][5]=points;
  //sort
  int l;
  for (l = 5 ; l > 0; l--)
    if (highscore[i][j][k][l] > highscore[i][j][k][l-1])
    {
      int points = highscore[i][j][k][l];
      char c1 = highscore_name[i][j][k][l][0];
      char c2 = highscore_name[i][j][k][l][1];
      char c3 = highscore_name[i][j][k][l][2];
      
      highscore[i][j][k][l] = highscore[i][j][k][l-1];
      highscore_name[i][j][k][l][0] = highscore_name[i][j][k][l-1][0];
      highscore_name[i][j][k][l][1] = highscore_name[i][j][k][l-1][1];
      highscore_name[i][j][k][l][2] = highscore_name[i][j][k][l-1][2];
      
      highscore[i][j][k][l-1] = points;
      highscore_name[i][j][k][l-1][0] = c1;
      highscore_name[i][j][k][l-1][1] = c2;
      highscore_name[i][j][k][l-1][2] = c3;
    }
    else
      break;
}

void settings_load()
{
  #ifdef REALGP2X
    settings_stone_quality = 0;
    settings_stars_rotating = 0;
  #else
    settings_stone_quality = 2;  
    settings_stars_rotating = 2;
  #endif
  settings_particles = 1;
  settings_font_quality = 1;
  settings_alpha_blending = 1;
  settings_volume = 100;
  settings_color = 4;
  settings_difficult = 0;
  settings_mode = 0; //highscore
  highscore_choice = 0;
  SDL_RWops *file=SDL_RWFromFile("./settings.dat","rb");
  if (file == NULL)
    return;
  SDL_RWread(file,&settings_stone_quality,sizeof(int),1);
  SDL_RWread(file,&settings_stars_rotating,sizeof(int),1);
  SDL_RWread(file,&settings_particles,sizeof(int),1);
  SDL_RWread(file,&settings_alpha_blending,sizeof(int),1);
  SDL_RWread(file,&settings_font_quality,sizeof(int),1);
  SDL_RWread(file,&settings_volume,sizeof(int),1);
  SDL_RWread(file,&settings_color,sizeof(int),1);
  SDL_RWread(file,&settings_difficult,sizeof(int),1);
  SDL_RWread(file,&settings_mode,sizeof(int),1);
  SDL_RWread(file,&highscore_choice,sizeof(int),1);
  SDL_RWclose(file);
}

void highscore_load()
{
  memset(highscore,0,2*6*10*6*sizeof(int));
  int i,j,k,l;
  for (i = 0; i < 2; i++)
    for (j = 0; j < 6; j++)
      for (k = 0; k < 10; k++)
        for (l = 0; l < 6; l++)
        {
          highscore_name[i][j][k][l][0]='Z';
          highscore_name[i][j][k][l][1]='I';
          highscore_name[i][j][k][l][2]='Z';
        }
  SDL_RWops *file=SDL_RWFromFile("./highscore.dat","rb");
  if (file == NULL)
    return;
  SDL_RWread(file,highscore,2*6*10*6*sizeof(int),1);
  SDL_RWread(file,highscore_name,2*6*10*6*3,1);
  SDL_RWclose(file);
}

void settings_save()
{
  SDL_RWops *file=SDL_RWFromFile("./settings.dat","wb");
  SDL_RWwrite(file,&settings_stone_quality,sizeof(int),1);
  SDL_RWwrite(file,&settings_stars_rotating,sizeof(int),1);
  SDL_RWwrite(file,&settings_particles,sizeof(int),1);
  SDL_RWwrite(file,&settings_alpha_blending,sizeof(int),1);
  SDL_RWwrite(file,&settings_font_quality,sizeof(int),1);
  SDL_RWwrite(file,&settings_volume,sizeof(int),1);
  SDL_RWwrite(file,&settings_color,sizeof(int),1);
  SDL_RWwrite(file,&settings_difficult,sizeof(int),1);
  SDL_RWwrite(file,&highscore_choice,sizeof(int),1);
  SDL_RWclose(file);
}

void highscore_save()
{
  SDL_RWops *file=SDL_RWFromFile("./highscore.dat","wb");
  SDL_RWwrite(file,highscore,2*6*10*6*sizeof(int),1);
  SDL_RWwrite(file,highscore_name,2*6*10*6*3,1);
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

int settings_get_alpha_blending()
{
  return settings_alpha_blending;
}

int settings_get_font_quality()
{
  return settings_font_quality;
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

void settings_set_alpha_blending(int value)
{
  settings_alpha_blending = value;
}

void settings_set_font_quality(int value)
{
  settings_font_quality = value;
}

void settings_set_volume(int value)
{
  settings_volume = value;
}

int settings_get_color()
{
  return settings_color;
}

int settings_get_difficult()
{
  return settings_difficult;
}

int settings_get_mode()
{
  return settings_mode;
}

void settings_set_color(int value)
{
  settings_color = value;
}

void settings_set_difficult(int value)
{
  settings_difficult = value;
}

void settings_set_mode(int value)
{
  settings_mode = value;
}

void settings_set_highscore_choice(int value)
{
  highscore_choice = value;
}

int settings_get_highscore_choice()
{
  return highscore_choice;
}

spFontPointer settings_get_font()
{
  return settings_font;
}

void settings_set_font(spFontPointer font)
{
  settings_font = font;
}

spFontPointer settings_get_small_font()
{
  return settings_small_font;
}

void settings_set_small_font(spFontPointer small_font)
{
  settings_small_font = small_font;
}

spFontPointer settings_get_middle_font()
{
  return settings_middle_font;
}

void settings_set_middle_font(spFontPointer middle_font)
{
  settings_middle_font = middle_font;
}
