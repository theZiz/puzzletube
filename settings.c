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
 of the GNU Lesser General Public license (the	"LGPL License"), in which case the	
 provisions of LGPL License are applicable instead of those									
 above.																																		 
																																						
 For feedback and questions about my Files and Projects please mail me,		 
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com												 
*/
#include "settings.h"
#include "../sparrow3d/sparrow3d.h"
#include <SDL.h>

#define SETTINGS_VERSION "5"
#define HIGHSCORE_VERSION "3"

int settings_stone_quality; //2 perfect, 1 okay, 0 gp2x
int settings_stars_rotating;
int settings_particles;
int settings_font_quality;
int settings_volume;
int settings_color;
int settings_mode;
int settings_control;
int settings_first_start;
int settings_language;
char settings_name[3];
spBundlePointer settings_translation;

spFontPointer settings_font;
spFontPointer settings_small_font;
spFontPointer settings_middle_font;
spFontPointer settings_countdown_font;
spFontPointer settings_highscore_font;

int highscore[3][2][3];
char highscore_name[3][2][3][3]; //game mode, normal/hard, top3
const int highscore_goal[3][2][4] =
//Points
{{{335570,250000,175000,100000},
  { 89589, 70000, 50000, 30000}},
//Survival
 {{ 1940,  1500,  1200,   900},
  { 1246,   900,   800,   700}},
//Race
 {{ 141,   300,   500,   700},
  { 1350,  2000,  3000,  4000}}};
  
int get_highscore_trophy(int game_mode,int dificult,int points)
{
	int i;
	for (i = 0; i < 4; i++)
		if (game_mode == 2)
		{
			if (highscore_goal[game_mode][dificult][i] >= points)
				return i;
		}
		else
			if (highscore_goal[game_mode][dificult][i] <= points)
				return i;
	return 4;
}

int get_next_highscore_trophy(int game_mode,int dificult,int points)
{
	int i = get_highscore_trophy(game_mode,dificult,points);
	printf("Your place is %i. Next better Place is %i.\n",i,i-1);
	if (i == 0)
		return 0;
	printf("Points %i\n",highscore_goal[game_mode][dificult][i-1]);
	return highscore_goal[game_mode][dificult][i-1];
}

char* get_highscore_name(int game_mode,int difficult,int rank)
{
	return highscore_name[game_mode][difficult][rank];
}

int get_highscore(int game_mode,int difficult,int rank)
{
	return highscore[game_mode][difficult][rank];
}

void insert_highscore(int game_mode,int difficult,char* name,int points)
{
	if (game_mode!=2 && highscore[game_mode][difficult][2] > points)
		return;
	if (game_mode==2 && highscore[game_mode][difficult][2] < points)
		return;
	highscore_name[game_mode][difficult][2][0]=name[0];
	highscore_name[game_mode][difficult][2][1]=name[1];
	highscore_name[game_mode][difficult][2][2]=name[2];
	highscore[game_mode][difficult][2]=points;
	//sort
	int l;
	for (l = 2 ; l > 0; l--)
		if ((game_mode!=2 && highscore[game_mode][difficult][l] > highscore[game_mode][difficult][l-1]) ||
				(game_mode==2 && highscore[game_mode][difficult][l] < highscore[game_mode][difficult][l-1]))
		{
			int points = highscore[game_mode][difficult][l];
			char c1 = highscore_name[game_mode][difficult][l][0];
			char c2 = highscore_name[game_mode][difficult][l][1];
			char c3 = highscore_name[game_mode][difficult][l][2];
			
			highscore[game_mode][difficult][l] = highscore[game_mode][difficult][l-1];
			highscore_name[game_mode][difficult][l][0] = highscore_name[game_mode][difficult][l-1][0];
			highscore_name[game_mode][difficult][l][1] = highscore_name[game_mode][difficult][l-1][1];
			highscore_name[game_mode][difficult][l][2] = highscore_name[game_mode][difficult][l-1][2];
			
			highscore[game_mode][difficult][l-1] = points;
			highscore_name[game_mode][difficult][l-1][0] = c1;
			highscore_name[game_mode][difficult][l-1][1] = c2;
			highscore_name[game_mode][difficult][l-1][2] = c3;
		}
		else
			break;
}

#ifdef GCW
char* get_path(char* buffer,char* file)
{
	sprintf(buffer,"%s/.puzzletube/%s",getenv("HOME"),file);
	return buffer;
}
#else
char* get_path(char* buffer,char* file)
{
	sprintf(buffer,".%s",file);
	return buffer;
}
#endif

void settings_load()
{
	#ifdef REALGP2X
		settings_stone_quality = 1;
	#else
		settings_stone_quality = 2;
	#endif
	settings_stars_rotating = 2;
	settings_particles = 2;
	settings_volume = 100;
	settings_color = 0;
	settings_control = 0; //simple control
	settings_mode = 2;
	settings_font = NULL;
	settings_middle_font = NULL;
	settings_small_font = NULL;
	settings_countdown_font = NULL;
	settings_highscore_font = NULL;
	settings_name[0]='A';
	settings_name[1]='A';
	settings_name[2]='A';
	settings_language = 0;
	settings_first_start = 1;
	char buffer[256];
	SDL_RWops *file=SDL_RWFromFile(get_path(buffer,"settings"SETTINGS_VERSION".dat"),"rb");
	if (file == NULL)
		return;
	SDL_RWread(file,&settings_stone_quality,sizeof(int),1);
	SDL_RWread(file,&settings_stars_rotating,sizeof(int),1);
	SDL_RWread(file,&settings_particles,sizeof(int),1);
	SDL_RWread(file,&settings_volume,sizeof(int),1);
	SDL_RWread(file,&settings_color,sizeof(int),1);
	SDL_RWread(file,&settings_control,sizeof(int),1);
	SDL_RWread(file,&settings_mode,sizeof(int),1);
	SDL_RWread(file,settings_name,sizeof(char)*3,1);
	SDL_RWread(file,&settings_language,sizeof(int),1);
	settings_first_start = 0;
	SDL_RWclose(file);
}

void highscore_load()
{
	int i,j,k,l;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 2; j++)
	for (l = 0; l < 3; l++)
	{
		highscore_name[i][j][l][0]='N';
		highscore_name[i][j][l][1]='U';
		highscore_name[i][j][l][2]='B';
		if (i == 2)
		highscore[i][j][l] = 10000;
		else
		highscore[i][j][l] = 0;
	}
	char buffer[256];
	SDL_RWops *file=SDL_RWFromFile(get_path(buffer,"highscore"HIGHSCORE_VERSION".dat"),"rb");
	if (file == NULL)
		return;
	SDL_RWread(file,highscore,3*2*3*sizeof(int),1);
	SDL_RWread(file,highscore_name,3*2*3*3,1);
	SDL_RWclose(file);
}

void settings_save()
{
	char buffer[256];
	spCreateDirectoryChain(get_path(buffer,""));
	SDL_RWops *file=SDL_RWFromFile(get_path(buffer,"settings"SETTINGS_VERSION".dat"),"wb");
	SDL_RWwrite(file,&settings_stone_quality,sizeof(int),1);
	SDL_RWwrite(file,&settings_stars_rotating,sizeof(int),1);
	SDL_RWwrite(file,&settings_particles,sizeof(int),1);
	SDL_RWwrite(file,&settings_volume,sizeof(int),1);
	SDL_RWwrite(file,&settings_color,sizeof(int),1);
	SDL_RWwrite(file,&settings_control,sizeof(int),1);
	SDL_RWwrite(file,&settings_mode,sizeof(int),1);
	SDL_RWwrite(file,settings_name,sizeof(char)*3,1);
	SDL_RWwrite(file,&settings_language,sizeof(int),1);
	SDL_RWclose(file);
}

void highscore_save()
{
	char buffer[256];
	spCreateDirectoryChain(get_path(buffer,""));
	SDL_RWops *file=SDL_RWFromFile(get_path(buffer,"highscore"HIGHSCORE_VERSION".dat"),"wb");
	SDL_RWwrite(file,highscore,3*2*3*sizeof(int),1);
	SDL_RWwrite(file,highscore_name,3*2*3*3,1);
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

int settings_get_color()
{
	return settings_color;
}

int settings_get_mode()
{
	return settings_mode;
}

int settings_get_control()
{
	return settings_control;
}

void settings_set_color(int value)
{
	settings_color = value;
}

void settings_set_mode(int value)
{
	settings_mode = value;
}

void settings_set_control(int value)
{
	settings_control = value;
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

spFontPointer settings_get_countdown_font()
{
	return settings_countdown_font;
}

void settings_set_countdown_font(spFontPointer countdown_font)
{
	settings_countdown_font = countdown_font;
}

spFontPointer settings_get_highscore_font()
{
	return settings_highscore_font;
}

void settings_set_highscore_font(spFontPointer highscore_font)
{
	settings_highscore_font = highscore_font;
}

void settings_reset_highscore_name(char* name)
{
	name[0]=settings_name[0];
	name[1]=settings_name[1];
	name[2]=settings_name[2];
}

void settings_set_highscore_name(char* name)
{
	settings_name[0]=name[0];
	settings_name[1]=name[1];
	settings_name[2]=name[2];
}

int settings_get_first_start()
{
	return settings_first_start;
}

void settings_set_language(int language)
{
	settings_language = language;
	spSetDefaultLanguage(spGetPossibleLanguage(language));
}

int settings_get_language()
{
	return settings_language;
}

void settings_set_translation(spBundlePointer translation)
{
	settings_translation = translation;
}

spBundlePointer settings_get_translation()
{
	return settings_translation;
}
