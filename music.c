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

#include "music.h"
#include "settings.h"
char musicName[256];
char musicAuthor[256];
int musicFade;
spSound *move_chunk,*rotating_chunk,*switch_chunk;
spSound *explosion_chunk[4];
int rotating_channel;
int move_channel;
spFileListPointer musicList = NULL;

void init_music()
{
	musicName[0]=0;
	spSoundInit();
	//spSoundSetChannels(32);
	move_chunk = spSoundLoad("./sounds/Vertical Movement.ogg");
	move_channel = -1;
	rotating_chunk = spSoundLoad("./sounds/Rotating.ogg");
	rotating_channel = -1;
	explosion_chunk[0] = spSoundLoad("./sounds/Explosion1.ogg");
	explosion_chunk[1] = spSoundLoad("./sounds/Explosion2.ogg");
	explosion_chunk[2] = spSoundLoad("./sounds/Explosion3.ogg");
	explosion_chunk[3] = spSoundLoad("./sounds/Explosion4.ogg");
	switch_chunk = spSoundLoad("./sounds/Tile Switch.ogg");
	set_volume(settings_get_volume());
	//Searching the folder
	spFileGetDirectory(&musicList,"./music",1,0);
	if (musicList)
		printf("Will play %i custom music files ingame!\n",musicList->count);
	
}

void move_sound_on()
{
	if (move_channel < 0)
		move_channel = spSoundPlay(move_chunk,-1,-1,0,0);
	else
		spSoundPause(0,move_channel);
}

void move_sound_off()
{
	if (move_channel >= 0)
		spSoundPause(1,move_channel);
}

void rotating_sound_on()
{
	if (rotating_channel < 0)
		rotating_channel = spSoundPlay(rotating_chunk,-1,-1,0,0);
	else
		spSoundPause(0,rotating_channel);
}

void rotating_sound_off()
{
	if (rotating_channel >= 0)
		spSoundPause(1,rotating_channel);
}

void set_volume(int volume)
{
	volume = volume * 128 / 100;
	spSoundSetMusicVolume(volume);
	spSoundSetVolume(volume);
}

int last_explosion = 0;

void play_explosion()
{
	if (last_explosion > 0)
		return;
	int number = rand()%4;
	spSoundPlay(explosion_chunk[number],-1,0,0,0);
	last_explosion = 200;
}

void play_switch()
{
	spSoundPlay(switch_chunk,-1,0,0,0);
}


void change_music(char* name,char* author)
{
	if (musicList)
		return;
	musicFade = 0;
	if (strcmp(musicName,name)==0)
		return;
	sprintf(musicName,"%s",name);
	sprintf(musicAuthor,"%s",author);
	char buffer[512];
	sprintf(buffer,"./sounds/%s.ogg",name);
	spSoundStopMusic(0);
	spSoundSetMusic(buffer);
	spSoundPlayMusic(0,-1);
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
	
	if (musicList)
	{
		sprintf(buffer,"\"%s\"",musicName);
		spFontDrawMiddle(engineWindowX/2,add-2*(small_font->maxheight),-1,buffer,small_font);
	}
	else
	{
		sprintf(buffer,"%s:",musicAuthor);
		spFontDrawMiddle(engineWindowX/2,add-2*(small_font->maxheight),-1,buffer,small_font);
		sprintf(buffer,"\"%s\"",musicName);
		spFontDrawMiddle(engineWindowX/2,add-1*(small_font->maxheight),-1,buffer,small_font);
	}
}

int volume_wait = 0;

void calc_music(int steps)
{ 
	if (last_explosion > 0)
		last_explosion -= steps;
	if (musicList && !Mix_PlayingMusic())
	{
		int nr = rand()%musicList->count;
		musicFade = 0;
		spFileListPointer track = musicList;
		int i;
		for (i = 0; i < nr; i++)
			track = track->next;
		sprintf(musicName,"%s",&(track->name[8]));
		char buffer[512];
		spSoundStopMusic(0);
		spSoundSetMusic(track->name);
		spSoundPlayMusic(0,0);
	}
	int i;
	for (i = 0;i<steps;i++)
	{
		/*volume_wait--;
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
		}*/
	}
	if (musicFade>5000)
		return;
	musicFade += steps;
}

void quit_music()
{
	spSoundDelete(move_chunk);
	spSoundDelete(rotating_chunk);
	spSoundDelete(explosion_chunk[0]);
	spSoundDelete(explosion_chunk[1]);
	spSoundDelete(explosion_chunk[2]);
	spSoundDelete(explosion_chunk[3]);
	spSoundDelete(switch_chunk);
	spSoundQuit();
}

