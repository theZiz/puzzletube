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
#include "game.h"
#include "lettering.h"
#include "settings.h"
#include "c4a.h"
#include "music.h"
#include <stdlib.h>
#include <string.h>

int playernumber;
int countdown;
int game_counter;
int insert_name;
int choosen_letter;
char myhighscore_name[3];

int star_add;

typedef SDL_Surface *PSDL_Surface;
PSDL_Surface stone_texture[9];
PSDL_Surface trophy_texture[4];
Sint32 w=0;

Sint32 posx[4],posy[4];
Sint32 pointposx[2*TIMEOUT],pointposy[2*TIMEOUT];
signed char is_change;
int pointstart;

int timeout;
char direction;

tstone stone[7][16];

int points;
int gameTime;
int timeStep;
int realTime;

#define START_TIME 120000
#define POINTS_GOAL 100000
#define LIKELYHOOD 8

pchange firstchange;
pparticle firstparticle;

char pause=0;

int chain;
int chain_break;
int oldchain;

typedef struct sPointVis *pPointVis;
typedef struct sPointVis {
	int points;
	int age;
	pPointVis next;
} tPointVis;

pPointVis first_pointVis;

void add_pointVis(int p)
{
	pPointVis mom = (pPointVis)malloc(sizeof(tPointVis));
	mom->points = p;
	mom->age = 0;
	mom->next = first_pointVis;
	first_pointVis = mom;
}

#define POINTAGE 4000

void calc_pointVis(int steps)
{
	pPointVis mom = first_pointVis;
	pPointVis last = NULL;
	while (mom)
	{
		mom->age += steps;
		if (mom->age > POINTAGE)
		{
			pPointVis next = mom->next;
			free(mom);
			if (last)
				last->next = next;
			else
				first_pointVis = next;
			mom = next;
		}
		last = mom;
		if (mom)
			mom = mom -> next;
	}
}

void delete_pointVis()
{
	while (first_pointVis)
	{
		pPointVis next = first_pointVis -> next;
		free(first_pointVis);
		first_pointVis = next;
	}
}

void test_and_set_chain()
{
	char found = 0;
	int x,y;
	for (x=0;x<16;x++)
		for (y=0;y<7;y++)
			if (stone[y][x].falling)
			{
				found = 1;
				break;
			}
	if (!found)
	{
		if (chain_break>0)
			chain_break--;
		else
			chain = 0;
	}
}

Sint32 get_type_color_h(int type,int w)
{
	if (type<6)
		return type*SP_PI/3;
	switch (type) //special stone
	{
		case 6: return w << 4;
		case 7: return (spSin(w*13)+(1<<SP_ACCURACY)>>SP_HALF_ACCURACY)*(SP_PI/12>>SP_HALF_ACCURACY);
	}
	return 0;
}

Uint8 get_type_color_s(int type,int w)
{
	if (type<6)
		return 255;
	switch (type) //special stone
	{
		case 6: return 32;
		case 7: return 255;
	}
	return 0;
}

Uint8 get_type_color_v(int type,int w)
{
	if (type==2) //color blindness optimization
		return 210;
	if (type<6)
		return 255;
	switch (type) //special stone
	{
		case 6: return 255;
		case 7: return ((spSin(w*64)*63)>>SP_ACCURACY)+96;
	}
	return 64;
}

#define GET_TYPE_COLOR(type,w) spGetHSV(get_type_color_h(type,w),get_type_color_s(type,w),get_type_color_v(type,w))

int last_win_type;

pwinsituation search_win_situation()
{
	pwinsituation finalresult = NULL;
	int x,y;
	for (x=0;x<16;x++)
		for (y=0;y<6;y++)
		{
			if (stone[y	][ x			].type==8 ||
					stone[y+1][ x			].type==8 ||
					stone[y	][(x+1)%16].type==8 ||
					stone[y+1][(x+1)%16].type==8)
				continue;
			if ((stone[y][x].type==6 && stone[y+1][x].type==6 && stone[y][(x+1)%16].type==6)
					||
					(stone[y][x].type==6 && stone[y+1][x].type==6 &&
					(stone[y][(x+1)%16].type==stone[y+1][(x+1)%16].type || stone[y+1][(x+1)%16].type==6))
					||
					(stone[y][x].type==6 &&
					(stone[y+1][x].type==stone[y	][(x+1)%16].type || stone[y	][(x+1)%16].type==6) &&
					(stone[y+1][x].type==stone[y+1][(x+1)%16].type || stone[y+1][(x+1)%16].type==6))
					||
					(stone[y][x].type>=0 &&
					(stone[y][x].type==stone[y+1][ x			].type || stone[y+1][ x			].type==6) &&
					(stone[y][x].type==stone[y	][(x+1)%16].type || stone[y	][(x+1)%16].type==6) &&
					(stone[y][x].type==stone[y+1][(x+1)%16].type || stone[y+1][(x+1)%16].type==6)))
			{
				pwinsituation result=(pwinsituation)malloc(sizeof(twinsituation));
				result->x=x;
				result->y=y;
				result->next=finalresult;
				char found=1;
				char noticed[7][16];
				memset(noticed,0,7*16);
				noticed[y][x]=1;
				int type=stone[y][x].type;
				if (type == 6)
					type = stone[y+1][x].type;
				if (type == 6)
					type = stone[y	][(x+1)%16].type;
				if (type == 6)
					type = stone[y+1][(x+1)%16].type;
				last_win_type = type;
				while (found)
				{
					found=0;
					pwinsituation situation=result;
					while (situation)
					{
						//left
						if (noticed[situation->y][(situation->x+15)%16]==0 &&
								(stone[situation->y][(situation->x+15)%16].type==type ||
								 stone[situation->y][(situation->x+15)%16].type==6))
						{
							pwinsituation newsituation=(pwinsituation)malloc(sizeof(twinsituation));
							newsituation->x=(situation->x+15)%16;
							newsituation->y=situation->y;
							newsituation->type=stone[situation->y][(situation->x+15)%16].type;
							newsituation->next=result;
							result=newsituation;
							found=1;
							noticed[situation->y][(situation->x+15)%16]=1;
						}

						//Right
						if (noticed[situation->y][(situation->x+1)%16]==0 &&
								(stone[situation->y][(situation->x+1)%16].type==type ||
								 stone[situation->y][(situation->x+1)%16].type==6))
						{
							pwinsituation newsituation=(pwinsituation)malloc(sizeof(twinsituation));
							newsituation->x=(situation->x+1)%16;
							newsituation->y=situation->y;
							newsituation->type=stone[situation->y][(situation->x+1)%16].type;
							newsituation->next=result;
							result=newsituation;
							found=1;
							noticed[situation->y][(situation->x+1)%16]=1;
						}

						//Down
						if (situation->y>0)
						{
							if (noticed[situation->y-1][situation->x]==0 &&
									(stone[situation->y-1][situation->x].type==type ||
									 stone[situation->y-1][situation->x].type==6))
							{
								pwinsituation newsituation=(pwinsituation)malloc(sizeof(twinsituation));
								newsituation->x=situation->x;
								newsituation->y=situation->y-1;
								newsituation->type=stone[situation->y-1][situation->x].type;
								newsituation->next=result;
								result=newsituation;
								found=1;
								noticed[situation->y-1][situation->x]=1;
							}
						}
						
						//Up
						if (situation->y<6)
						{
							if (noticed[situation->y+1][situation->x]==0 &&
									(stone[situation->y+1][situation->x].type==type ||
									 stone[situation->y+1][situation->x].type==6))
							{
								pwinsituation newsituation=(pwinsituation)malloc(sizeof(twinsituation));
								newsituation->x=situation->x;
								newsituation->y=situation->y+1;
								newsituation->type=stone[situation->y+1][situation->x].type;
								newsituation->next=result;
								result=newsituation;
								found=1;
								noticed[situation->y+1][situation->x]=1;
							}
						}

						//Behind
						if (noticed[situation->y][(situation->x+8)%16]==0 &&
								stone[situation->y][(situation->x+8)%16].type==type)
						{
							pwinsituation newsituation=(pwinsituation)malloc(sizeof(twinsituation));
							newsituation->x=(situation->x+8)%16;
							newsituation->y=situation->y;
							newsituation->type=stone[situation->y][(situation->x+8)%16].type;
							newsituation->next=result;
							result=newsituation;
							found=1;
							noticed[situation->y][(situation->x+8)%16]=1;
						}
						
						situation=situation->next;
					}
				}
				if (type==7)
				{
					pwinsituation situation=result;
					while (situation!=finalresult)
					{
						if (stone[situation->y][situation->x].type==7)
						{
							//left
							if (noticed[situation->y][(situation->x+15)%16]==0)
							{
								pwinsituation newsituation=(pwinsituation)malloc(sizeof(twinsituation));
								newsituation->x=(situation->x+15)%16;
								newsituation->y=situation->y;
								newsituation->next=result;
								newsituation->type=stone[situation->y][(situation->x+15)%16].type;
								result=newsituation;
								found=1;
								noticed[situation->y][(situation->x+15)%16]=1;
							}

							//Right
							if (noticed[situation->y][(situation->x+1)%16]==0)
							{
								pwinsituation newsituation=(pwinsituation)malloc(sizeof(twinsituation));
								newsituation->x=(situation->x+1)%16;
								newsituation->y=situation->y;
								newsituation->type=stone[situation->y][(situation->x+1)%16].type;
								newsituation->next=result;
								result=newsituation;
								found=1;
								noticed[situation->y][(situation->x+1)%16]=1;
							}

							//Down
							if (situation->y>0)
							{
								if (noticed[situation->y-1][situation->x]==0)
								{
									pwinsituation newsituation=(pwinsituation)malloc(sizeof(twinsituation));
									newsituation->x=situation->x;
									newsituation->y=situation->y-1;
									newsituation->type=stone[situation->y-1][situation->x].type;
									newsituation->next=result;
									result=newsituation;
									found=1;
									noticed[situation->y-1][situation->x]=1;
								}
							}
							
							//Up
							if (situation->y<6)
							{
								if (noticed[situation->y+1][situation->x]==0)
								{
									pwinsituation newsituation=(pwinsituation)malloc(sizeof(twinsituation));
									newsituation->x=situation->x;
									newsituation->y=situation->y+1;
									newsituation->type=stone[situation->y+1][situation->x].type;
									newsituation->next=result;
									result=newsituation;
									found=1;
									noticed[situation->y+1][situation->x]=1;
								}
							}
						}
						situation=situation->next;
					}
				}
				finalresult = result;
			}
		}
	return finalresult;
}

void delete_win_situation(pwinsituation situation)
{
	while (situation!=NULL)
	{
		pwinsituation temp = situation->next;
		free(situation);
		situation=temp;
	}
}

void remove_win_situations()
{
	pwinsituation situation;
	while ((situation=search_win_situation())!=NULL)
	{
		pwinsituation temp=situation;
		while (temp!=NULL)
		{
			if (settings_get_color())
			{
				if (rand()%LIKELYHOOD < LIKELYHOOD-1)
					stone[temp->y][temp->x].type=rand()%6;
				else
					stone[temp->y][temp->x].type=rand()%3+6;
			}
			else
			{
				if (rand()%LIKELYHOOD < LIKELYHOOD-1)
					stone[temp->y][temp->x].type=rand()%4;
				else
					stone[temp->y][temp->x].type=rand()%3+6;
			}
			
			stone[temp->y][temp->x].h=get_type_color_h(stone[temp->y][temp->x].type,w);
			stone[temp->y][temp->x].s=get_type_color_s(stone[temp->y][temp->x].type,w);
			stone[temp->y][temp->x].v=get_type_color_v(stone[temp->y][temp->x].type,w);
			temp=temp->next; 
		}
	}
	delete_win_situation(situation);
}

void init_light()
{
	/*plight light=engineGetLightPointer();	
	light[0].r=255;
	light[0].g=255;
	light[0].b=255;*/
}


void prepare_game_objects(char complete)
{
	if (!complete)
	{
		points = 0;
		gameTime = START_TIME;
		realTime = 0;
		chain = 0;
		int a,y;
		for (y=0;y<7;y++)
			for (a=0;a<16;a++)
			{
				if (settings_get_color())
				{
					if (rand()%LIKELYHOOD < LIKELYHOOD-1)
						stone[y][a].type=rand()%6;
					else
						stone[y][a].type=rand()%3+6;
				}
				else
				{
					if (rand()%LIKELYHOOD < LIKELYHOOD-1)
						stone[y][a].type=rand()%4;
					else
						stone[y][a].type=rand()%3+6;
				}
				stone[y][a].h=get_type_color_h(stone[y][a].type,w);
				stone[y][a].s=get_type_color_s(stone[y][a].type,w);
				stone[y][a].v=get_type_color_v(stone[y][a].type,w);
				stone[y][a].falling=0;
				stone[y][a].new=0;
			}
		if (settings_get_control() == 1)
		{
			posx[0]=0;
			posy[0]=-2<<SP_ACCURACY;
			posx[3]=posx[0];
			posy[3]=posy[0];
			posx[1]=0;
			posy[1]=0;
			posx[2]=0;
			posy[2]=2<<SP_ACCURACY;
		}
		else
		{
			posx[0]=0;
			posy[0]=0;
		}
		int i;
		for (i=0;i<2*TIMEOUT;i++)
		{
			pointposx[i]=0; 
			pointposy[i]=(i-TIMEOUT<<SP_ACCURACY)/(TIMEOUT/2); 
		}
		is_change=0;
		pointstart=0;
		timeout=0;
		direction=2;
		firstchange=NULL;
		firstparticle=NULL;
		remove_win_situations();
		init_light();
		delete_all_lettering();
		delete_all_bordering();
	}
	else
	{
		int i;
		char buffer[256];
		for (i = 0; i < 9; i++)
		{
			sprintf(buffer,"./images/stone%i.png",i+1);
			stone_texture[i] = spLoadSurface(buffer);
		}
		for (i = 0; i < 4; i++)
		{
			sprintf(buffer,"./images/trophy%i.png",i);
			trophy_texture[i] = spLoadSurface(buffer);
		}
		resize_particle(spGetWindowSurface()->w,spGetWindowSurface()->h);
	}
}

void delete_game_objects()
{
	int i;
	for (i = 0;i<9;i++)
		SDL_FreeSurface(stone_texture[i]);
	for (i = 0;i<4;i++)
		SDL_FreeSurface(trophy_texture[i]);
}

void new_particle(Sint32 x,Sint32 y,Sint32 z,Sint32 h,Uint8 s,Uint8 v,Sint32 rotx,Sint32 roty,Sint32 rotz)
{
	pparticle particle=(pparticle)malloc(sizeof(tparticle));
	particle->x=x;
	particle->y=y;
	particle->z=z;
	particle->h=h;
	particle->s=s;
	particle->v=v;
	particle->rotx=rotx;
	particle->roty=roty;
	particle->rotz=rotz;
	particle->dx=rand()%(1<<SP_ACCURACY-7)-(1<<SP_ACCURACY-8);
	particle->dy=rand()%(1<<SP_ACCURACY-7)-(1<<SP_ACCURACY-8);
	particle->dz=rand()%(1<<SP_ACCURACY-7)-(1<<SP_ACCURACY-8);
	particle->age=PARTICLE_AGE;
	particle->next=firstparticle;
	firstparticle=particle;
}

void step_particles()
{
	pparticle last=NULL;
	pparticle particle=firstparticle;
	while (particle)
	{
		particle->dy-=1<<SP_ACCURACY-15;
		particle->x+=particle->dx;
		particle->y+=particle->dy;
		particle->z+=particle->dz;
		particle->age--;
		if (particle->age<=0)
		{
			if (last)
				last->next=particle->next;
			else
				firstparticle=particle->next;
			free(particle);
			particle=last;
		}
		last=particle;
		if (particle)
			particle=particle->next;
	}
}

void initate_change(int ax,int ay,int bx,int by)
{
	pchange newchange=(pchange)malloc(sizeof(tchange));
	newchange->progress=CHANGE_TIME;
	newchange->ax=ax;
	newchange->ay=ay;
	newchange->bx=bx;
	newchange->by=by;
	newchange->next=firstchange;
	firstchange=newchange;
}

void step_changes()
{
	pchange lastchange=NULL;
	pchange change=firstchange;
	while (change!=NULL)
	{
		change->progress--;
		if (change->progress==0)
		{
			tstone temp=stone[change->ay][change->ax];
			stone[change->ay][change->ax]=stone[change->by][change->bx];
			stone[change->by][change->bx]=temp;
			free(change);
			if (lastchange==NULL)
				firstchange=NULL;
			else
				lastchange->next=NULL;
			break;
		}
		lastchange=change;
		change=change->next; 
	}
}

pchange is_in_change(int x,int y)
{
	pchange change=firstchange;
	while (change!=NULL)
	{
		if (change->ax==x && change->ay==y)
			return change;
		if (change->bx==x && change->by==y)
			return change;
		change=change->next; 
	}
	return NULL;
}


void make_win_situations_invalid()
{
	spBundlePointer translation = settings_get_translation();
	spFontPointer font = settings_get_font();
	spFontPointer small_font = settings_get_small_font();
	spFontPointer middle_font = settings_get_middle_font();
	int new_points = 0;
	pwinsituation situation;
	char found = 0;
	char type_found[12];
	while ((situation=search_win_situation())!=NULL)
	{
		memset(type_found,0,12);
		//type_found[6]=1;
		char count = 0;
		found = 1;
		pwinsituation temp=situation;
		int i = 0;
		while (temp!=NULL)
		{
			if (stone[temp->y][temp->x].type>=0 && type_found[stone[temp->y][temp->x].type] == 0)
			{
				printf("__%i__\n",stone[temp->y][temp->x].type);
				add_bordering(situation,GET_TYPE_COLOR(stone[temp->y][temp->x].type,w));
				count++;
				type_found[stone[temp->y][temp->x].type] = 1;
			}

			stone[temp->y][temp->x].type=-1;
			play_explosion();
			if (settings_get_particles() == 2)
			{
				new_particle(spCos((temp->x*SP_PI>>3)-SP_PI/32)*5,((temp->y-3)*2<<SP_ACCURACY)-(1<<SP_ACCURACY-1),spSin((temp->x*SP_PI>>3)-SP_PI/32)*5,
										 stone[temp->y][temp->x].h,stone[temp->y][temp->x].s,stone[temp->y][temp->x].v,
										 0,2*SP_PI+SP_PI/2-(temp->x*SP_PI>>3),0);
				new_particle(spCos((temp->x*SP_PI>>3)+SP_PI/32)*5,((temp->y-3)*2<<SP_ACCURACY)-(1<<SP_ACCURACY-1),spSin((temp->x*SP_PI>>3)+SP_PI/32)*5,
										 stone[temp->y][temp->x].h,stone[temp->y][temp->x].s,stone[temp->y][temp->x].v,
										 0,2*SP_PI+SP_PI/2-(temp->x*SP_PI>>3),0);
				new_particle(spCos((temp->x*SP_PI>>3)-SP_PI/32)*5,((temp->y-3)*2<<SP_ACCURACY)+(1<<SP_ACCURACY-1),spSin((temp->x*SP_PI>>3)-SP_PI/32)*5,
										 stone[temp->y][temp->x].h,stone[temp->y][temp->x].s,stone[temp->y][temp->x].v,
										 0,2*SP_PI+SP_PI/2-(temp->x*SP_PI>>3),0);
				new_particle(spCos((temp->x*SP_PI>>3)+SP_PI/32)*5,((temp->y-3)*2<<SP_ACCURACY)+(1<<SP_ACCURACY-1),spSin((temp->x*SP_PI>>3)+SP_PI/32)*5,
										 stone[temp->y][temp->x].h,stone[temp->y][temp->x].s,stone[temp->y][temp->x].v,
										 0,2*SP_PI+SP_PI/2-(temp->x*SP_PI>>3),0);
			}
			else
			if (settings_get_particles() == 1)
			{
				new_particle(spCos((temp->x*SP_PI>>3))*5,((temp->y-3)*2<<SP_ACCURACY),spSin((temp->x*SP_PI>>3))*5,
										 stone[temp->y][temp->x].h,stone[temp->y][temp->x].s,stone[temp->y][temp->x].v,
										 0,2*SP_PI+SP_PI/2-(temp->x*SP_PI>>3),0);
			}
			i++;
			printf("%i:%i with type %i\n",temp->x,temp->y,temp->type);
			temp=temp->next; 
		}
			if (i>0)
			{
				new_points = (int)(pow((float)(i),1.5)*100.0);
				char buffer[32];
				sprintf(buffer,"+%i (%i)",new_points,i);
				add_line();
				add_lettering(buffer,small_font);
			}
			if (i<7)
				{}
			else
			if (i<10)
				add_lettering(spGetTranslationFromCaption(translation,"Well!"),middle_font);
			else
			if (i<14)
				add_lettering(spGetTranslationFromCaption(translation,"Great!"),middle_font);
			else
			if (i<19)
				add_lettering(spGetTranslationFromCaption(translation,"Fantastic!"),middle_font);
			else
			if (i<25)
				add_lettering(spGetTranslationFromCaption(translation,"Incredible!"),middle_font);
			delete_win_situation(situation);
			if (count>1)
			{
				add_lettering("+15 %",small_font);
				add_lettering(spGetTranslationFromCaption(translation,"Combo!"),middle_font);
				new_points = new_points * 115/100;
			}
	}
	if (found)
	{
		chain++;
		chain_break=100;
		if (chain>1)
		{
			char buffer[256];
			sprintf(buffer,"+%i %%",(chain-1)*10);
			add_lettering(buffer,small_font);
			switch (chain)
			{
				case 2: add_lettering(spGetTranslationFromCaption(translation,"Chain!"),middle_font);break;
				case 3: add_lettering(spGetTranslationFromCaption(translation,"2x Chain!"),middle_font); break;
				case 4: add_lettering(spGetTranslationFromCaption(translation,"3x Chain!"),middle_font); break;
				case 5: add_lettering(spGetTranslationFromCaption(translation,"4x Chain!"),middle_font); break;
				case 6: add_lettering(spGetTranslationFromCaption(translation,"5x Chain!"),middle_font); break;
				default: add_lettering(spGetTranslationFromCaption(translation,"Mega Chain!"),middle_font); break;
			}
			new_points = new_points * (9+chain) / 10;
		}
		if (new_points>0)
			add_pointVis(new_points);
		if (settings_get_mode() == 1)
		{
			gameTime += new_points*5;
			if (gameTime > START_TIME)
				gameTime = START_TIME;
		}
		else
		{
			points += new_points;
			printf("===%i===\n",points);
		}
	}
}

#define PARTICLE_SPEED 1500

void draw_particle(int posx,int posy,Sint32 r,int time,SDL_Surface* particle)
{
	spSetAlphaPattern4x4(PARTICLE_ALPHA,0);
	Sint32* modellViewMatrix=spGetMatrix();
	Sint32 matrix[16];
	memcpy(matrix,modellViewMatrix,sizeof(Sint32)*16);
		spTranslate(spCos(-(posx>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,posy,spSin((posx>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2);
		spRotate(0,1<<SP_ACCURACY,0,(posx>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1));
		if (time<PARTICLE_SPEED/4)
			spBlit3D(r*time/(PARTICLE_SPEED/8)-r,r,0,particle);
		else
		if (time<PARTICLE_SPEED/2)
			spBlit3D(r,r-r*(time-PARTICLE_SPEED/4)/(PARTICLE_SPEED/8),0,particle);
		else
		if (time<3*PARTICLE_SPEED/4)
			spBlit3D(r-r*(time-PARTICLE_SPEED/2)/(PARTICLE_SPEED/8),-r,0,particle);
		else
			spBlit3D(-r,r-r*(PARTICLE_SPEED-time)/(PARTICLE_SPEED/8),0,particle);
		
	memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);
	spDeactivatePattern();
}

#define PARTICLE_SPEED2 2500

void draw_particle2(int posx,int posy,Sint32 r,int time,SDL_Surface* particle)
{
	spSetAlphaPattern4x4(PARTICLE_ALPHA,0);
	Sint32* modellViewMatrix=spGetMatrix();
	Sint32 matrix[16];
	memcpy(matrix,modellViewMatrix,sizeof(Sint32)*16);
		spTranslate(spCos(-(posx>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,posy,spSin((posx>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2);
		spRotate(0,1<<SP_ACCURACY,0,(posx>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1));
		if (time<PARTICLE_SPEED2/4)
			spBlit3D(r-r*time/(PARTICLE_SPEED2/8),r,0,particle);
		else
		if (time<PARTICLE_SPEED2/2)
			spBlit3D(r,r*(time-PARTICLE_SPEED2/4)/(PARTICLE_SPEED2/8)-r,0,particle);
		else
		if (time<3*PARTICLE_SPEED2/4)
			spBlit3D(r*(time-PARTICLE_SPEED2/2)/(PARTICLE_SPEED2/8)-r,-r,0,particle);
		else
			spBlit3D(-r,r*(PARTICLE_SPEED2-time)/(PARTICLE_SPEED2/8)-r,0,particle);
		
	memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);
	spDeactivatePattern();
}

int last_rotate = 0;
int choose_one = 0; //1 A, 2 B, 3 X, 4 Y

void draw_stone(int type,int h,int s,int v,int a,Sint32 posx_zero,int w, int special)
{
	if (special)
	{
		Uint16 input = spGetHSV(h,s,v);
		Uint16 color = spGetHSV(0,0,255-96+(3*spSin((-posx_zero>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)-((a+8)*SP_PI>>3))>>(SP_ACCURACY-5)));
		Uint16 output = ((input * color >> 16) & 63488)
									+ (((input & 2047) * (color & 2047) >> 11) & 2016)
									+ ((input & 31) * (color & 31) >> 5);
		spQuad3D(0,+SP_ONE/2,0,
						 0,-SP_ONE/2,0,
						 0,-SP_ONE/8,-SP_ONE*5,
						 0,+SP_ONE/8,-SP_ONE*5,output);
		spQuad3D(+SP_ONE/2,0,0,
						 -SP_ONE/2,0,0,
						 -SP_ONE/8,0,-SP_ONE*5,
						 +SP_ONE/8,0,-SP_ONE*5,output);
	}
	if (settings_get_stone_quality() == 2)
	{
		spBindTexture(stone_texture[type]);
		spQuadTex3D(-7<<SP_ACCURACY-3,-7<<SP_ACCURACY-3,0,0,stone_texture[0]->h-SP_FONT_EXTRASPACE-1,
								 7<<SP_ACCURACY-3,-7<<SP_ACCURACY-3,0,stone_texture[0]->w-SP_FONT_EXTRASPACE-1,stone_texture[0]->h-SP_FONT_EXTRASPACE-1,
								 7<<SP_ACCURACY-3, 7<<SP_ACCURACY-3,0,stone_texture[0]->w-SP_FONT_EXTRASPACE-1,0,
								-7<<SP_ACCURACY-3, 7<<SP_ACCURACY-3,0,0,0,spGetHSV(0,0,255-64+(2*spSin((-posx_zero>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)-((a+8)*SP_PI>>3))>>(SP_ACCURACY-5))));
	}
	else
	if (settings_get_stone_quality() == 1)
	{
		Uint16 input = spGetHSV(h,s,v);
		Uint16 color = spGetHSV(0,0,255-96+(3*spSin((-posx_zero>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)-((a+8)*SP_PI>>3))>>(SP_ACCURACY-5)));
		Uint16 output = ((input * color >> 16) & 63488)
									+ (((input & 2047) * (color & 2047) >> 11) & 2016)
									+ ((input & 31) * (color & 31) >> 5);
		input = spGetHSV(h,s*3/4,v);
		Uint16 lighter = ((input * color >> 16) & 63488)
									 + (((input & 2047) * (color & 2047) >> 11) & 2016)
									 + ((input & 31) * (color & 31) >> 5);
		input = spGetHSV(h,s,v*3/4);
		Uint16 dark = ((input * color >> 16) & 63488)
								+ (((input & 2047) * (color & 2047) >> 11) & 2016)
								+ ((input & 31) * (color & 31) >> 5);
		input = spGetHSV(h,s,v*2/4);
		Uint16 darker = ((input * color >> 16) & 63488)
									+ (((input & 2047) * (color & 2047) >> 11) & 2016)
									+ ((input & 31) * (color & 31) >> 5);
		spQuad3D(-5<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,
							5<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,
							5<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,
						 -5<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,output);
		spQuad3D(-7<<SP_ACCURACY-3,-7<<SP_ACCURACY-3,0,
						 -5<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,
						 -5<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,
						 -7<<SP_ACCURACY-3, 7<<SP_ACCURACY-3,0,dark);
		spQuad3D( 7<<SP_ACCURACY-3,-7<<SP_ACCURACY-3,0,
							5<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,
							5<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,
							7<<SP_ACCURACY-3, 7<<SP_ACCURACY-3,0,dark);
		spQuad3D(-7<<SP_ACCURACY-3,-7<<SP_ACCURACY-3,0,
							7<<SP_ACCURACY-3,-7<<SP_ACCURACY-3,0,
							5<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,
						 -5<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,darker);
		spQuad3D(-7<<SP_ACCURACY-3, 7<<SP_ACCURACY-3,0,
							7<<SP_ACCURACY-3, 7<<SP_ACCURACY-3,0,
							5<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,
						 -5<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,lighter);
	}
	else
	{
		Uint16 input = spGetHSV(h,s,v);
		Uint16 color = spGetHSV(0,0,255-96+(3*spSin((-posx_zero>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)-((a+8)*SP_PI>>3))>>(SP_ACCURACY-5)));
		Uint16 output = ((input * color >> 16) & 63488)
									+ (((input & 2047) * (color & 2047) >> 11) & 2016)
									+ ((input & 31) * (color & 31) >> 5);
		spQuad3D(-7<<SP_ACCURACY-3,-7<<SP_ACCURACY-3,0,
							7<<SP_ACCURACY-3,-7<<SP_ACCURACY-3,0,
							7<<SP_ACCURACY-3, 7<<SP_ACCURACY-3,0,
						 -7<<SP_ACCURACY-3, 7<<SP_ACCURACY-3,0,output);
	}	
}

void draw_game(void)
{
	spBundlePointer translation = settings_get_translation();
	if (settings_get_stone_quality() != 2)
			spSetAlphaTest(0);
	spFontPointer font = settings_get_font();
	spFontPointer small_font = settings_get_small_font();
	spFontPointer middle_font = settings_get_middle_font();
	spFontPointer countdown_font = settings_get_countdown_font();
	spFontPointer highscore_font = settings_get_highscore_font();
	SDL_Surface* screen = spGetWindowSurface();
	Sint32* modellViewMatrix=spGetMatrix();
	//plight light=engineGetLightPointer();
	int engineWindowX=spGetWindowSurface()->w;
	int engineWindowY=spGetWindowSurface()->h;

	//spResetZBuffer();
	spIdentity();
		
	Sint32 matrix[16];
	
	spTranslate(0,0,-20<<SP_ACCURACY);

	if (settings_get_stars_rotating()==0)
		spClearTarget(BACKGROUND_COLOR);

	//Stars
	spSetZSet(0);
	spSetZTest(0);
	if (settings_get_stars_rotating()==1)
		draw_stars(star_add);
	else
	if (settings_get_stars_rotating()==2)
		draw_stars((-posx[0]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+star_add);
	
	
	spRotate(0,1<<SP_ACCURACY,0,(-posx[0]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1));
	last_rotate = (-posx[0]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1);
 
	int add=1;
	if (is_change>0)
	{
		if (is_change<2)
			add++;
		spRotate(0,-1<<SP_ACCURACY,0,((timeout<<SP_HALF_ACCURACY-1)/TIMEOUT)*(SP_PI>>SP_HALF_ACCURACY+add));
		last_rotate += ((timeout<<SP_HALF_ACCURACY-1)/TIMEOUT)*(SP_PI>>SP_HALF_ACCURACY+add);
	}
	if (is_change<0)
	{
		if (is_change>-2)
			add++;
		spRotate(0, 1<<SP_ACCURACY,0,((timeout<<SP_HALF_ACCURACY-1)/TIMEOUT)*(SP_PI>>SP_HALF_ACCURACY+add));
		last_rotate += ((timeout<<SP_HALF_ACCURACY-1)/TIMEOUT)*(SP_PI>>SP_HALF_ACCURACY+add);
	}

	//spTranslate(0,spSin(w<<3)>>2,0);

	spRotate(1<<SP_ACCURACY,0,0,(spSin(w<<3)>>SP_HALF_ACCURACY)*(SP_PI>>SP_HALF_ACCURACY)>>5);
	spRotate(0,1<<SP_ACCURACY,0,(spCos(w<<3)>>SP_HALF_ACCURACY)*(SP_PI>>SP_HALF_ACCURACY)>>5);
	
	int meta_a,a,y;
	//Drawing from the back to the front
	spSetZTest(0);
	spSetZSet(0);
	int left_side = 1;
	//for (meta_a=16-(posx[0]>>SP_ACCURACY)+15;meta_a>=16-(posx[0]>>SP_ACCURACY);meta_a--)
	meta_a=16-(posx[0]+SP_ONE/2>>SP_ACCURACY)+12; //back
	int left_a = meta_a;
	int right_a = meta_a;
	int i;
	for (i = 0; i < 16; i++)
	{
		a = meta_a & 15;
		int loop_y;
		for (loop_y = 0; loop_y < 7; loop_y++)
		//for (y=-6;y<=6;y+=2)
		{
			switch (loop_y)
			{
				case 0: y = -6; break;
				case 1: y = +6; break;
				case 2: y = -4; break;
				case 3: y = +4; break;
				case 4: y = -2; break;
				case 5: y = +2; break;
				case 6: y =  0; break;
			}
			memcpy(matrix,modellViewMatrix,sizeof(Sint32)*16);
			pchange change=is_in_change(a,3+y/2);
			Sint32 px=spCos(a*SP_PI>>3)*5;
			Sint32 py=y<<SP_ACCURACY;
			Sint32 pz=spSin(a*SP_PI>>3)*5;
			if (change)
			{
				int sign=1;
				Sint32 new_a=a<<SP_ACCURACY;
				Sint32 new_y=y<<SP_ACCURACY;
				if (change->bx==a && change->by==3+y/2)
					sign=-1;
				switch (change->ax-change->bx)
				{
					case	14: 
					case - 2: new_a+=sign*2*((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
					case	15:
					case - 1: new_a+=sign*	((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
					case -15: 
					case	 1: new_a-=sign*	((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
					case -14:
					case	 2: new_a-=sign*2*((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
				}
				switch (change->ay-change->by)
				{
					case -2: new_y+=sign*4*((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
					case -1: new_y+=sign*2*((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
					case	1: new_y-=sign*2*((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
					case	2: new_y-=sign*4*((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
				}
				px=(spCos((new_a>>SP_HALF_ACCURACY+1)*(SP_PI>>SP_HALF_ACCURACY+2))>>SP_HALF_ACCURACY)*((1<<SP_ACCURACY)-sign*(spSin(change->progress*SP_PI/CHANGE_TIME)/5)>>SP_HALF_ACCURACY)*5;
				py=new_y;
				pz=(spSin((new_a>>SP_HALF_ACCURACY+1)*(SP_PI>>SP_HALF_ACCURACY+2))>>SP_HALF_ACCURACY)*((1<<SP_ACCURACY)-sign*(spSin(change->progress*SP_PI/CHANGE_TIME)/5)>>SP_HALF_ACCURACY)*5;
			}
			spTranslate(px,py,pz);
			spRotate(0,1<<SP_ACCURACY,0,2*SP_PI+SP_PI/2-(a*SP_PI>>3));
			if (settings_get_borders())
			{
				spSetAlphaTest(0);
				pbordering bordering = get_first_bordering();
				while (bordering)
				{
					spSetBlending(bordering->alpha);
					int Y = (y>>1)+3;
					if (bordering->vertical_line[Y][a] >= 0)
						spQuad3D( SP_ONE-SP_ONE/8,+SP_ONE,0,
											SP_ONE+SP_ONE/8,+SP_ONE,0,
											SP_ONE+SP_ONE/8,-SP_ONE,0,
											SP_ONE-SP_ONE/8,-SP_ONE,0,bordering->vertical_line[(y>>1)+3][a]);
					if (bordering->horizental_line[Y][a] >= 0)
						spQuad3D(-SP_ONE,-SP_ONE+SP_ONE/8,0,
										 +SP_ONE,-SP_ONE+SP_ONE/8,0,
										 +SP_ONE,-SP_ONE-SP_ONE/8,0,
										 -SP_ONE,-SP_ONE-SP_ONE/8,0,bordering->horizental_line[(y>>1)+3][a]);
					if (Y == 6)
					{
						if (bordering->horizental_line[7][a] >= 0)
							spQuad3D(-SP_ONE, SP_ONE+SP_ONE/8,0,
											 +SP_ONE, SP_ONE+SP_ONE/8,0,
											 +SP_ONE, SP_ONE-SP_ONE/8,0,
											 -SP_ONE, SP_ONE-SP_ONE/8,0,bordering->horizental_line[7][a]);					
					}
					bordering = bordering->next;
				}
				spSetBlending(SP_ONE);
				spSetAlphaTest(1);
			}
			if (stone[(y>>1)+3][a].falling)
				spTranslate(0,-(2*(FALL_TIME-stone[(y>>1)+3][a].falling)<<SP_ACCURACY)/FALL_TIME,0);
			if (stone[(y>>1)+3][a].type<0)
			{
				memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);
				continue;
			}
			stone[(y>>1)+3][a].h = get_type_color_h(stone[(y>>1)+3][a].type,w);
			stone[(y>>1)+3][a].s = get_type_color_s(stone[(y>>1)+3][a].type,w);
			stone[(y>>1)+3][a].v = get_type_color_v(stone[(y>>1)+3][a].type,w);
			int s=stone[(y>>1)+3][a].s;

				//spRotate(0,0,1<<SP_ACCURACY,SP_PI/4);

			int v=stone[(y>>1)+3][a].v;//-64+(2*spSin((-posx[0]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)-((a+8)*SP_PI>>3))>>(SP_ACCURACY-5));
			
			if (stone[(y>>1)+3][a].new)
			{
				spScale((NEW_TIME-stone[(y>>1)+3][a].new<<SP_ACCURACY)/NEW_TIME,(NEW_TIME-stone[(y>>1)+3][a].new<<SP_ACCURACY)/NEW_TIME,(NEW_TIME-stone[(y>>1)+3][a].new<<SP_ACCURACY)/NEW_TIME);
				
			}

			if (v<0)
				v=0;
			if (v>255)
				v=255;
			if (s<0)
				s=0;
			if (s>255)
				s=255;
			int special = 0;
			if (stone[(y>>1)+3][a].type==stone[(y>>1)+3][(a+8)%16].type)
			{
				spRotate(0,0,1<<SP_ACCURACY,spSin(w*64)/2);
				special = 1;
			}
			draw_stone(stone[(y>>1)+3][a].type,stone[(y>>1)+3][a].h,s,v,a,posx[0],w,special);
			memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);
		}
		if (left_side)
		{
			left_a--;
			meta_a = left_a;
		}
		else
		{
			right_a++;
			meta_a = right_a;
		}
		left_side=1-left_side;
	}
	//particles
	spSetAlphaTest(0);
	pparticle particle=firstparticle;
	int multiplier = 1;
	if (settings_get_particles() == 1)
		multiplier = 2;
	while (particle)
	{
		memcpy(matrix,modellViewMatrix,sizeof(Sint32)*16);
		spTranslate(particle->x,particle->y,particle->z);
		spRotate(1<<SP_ACCURACY,0,0,particle->rotx);
		spRotate(0,1<<SP_ACCURACY,0,particle->roty);
		spRotate(0,0,1<<SP_ACCURACY,particle->rotz);
		spQuad3D(-(particle->age<<SP_ACCURACY)/PARTICLE_AGE/4*multiplier,
						 -(particle->age<<SP_ACCURACY)/PARTICLE_AGE/4*multiplier,0,
							(particle->age<<SP_ACCURACY)/PARTICLE_AGE/4*multiplier,
						 -(particle->age<<SP_ACCURACY)/PARTICLE_AGE/4*multiplier,0,
							(particle->age<<SP_ACCURACY)/PARTICLE_AGE/4*multiplier,
							(particle->age<<SP_ACCURACY)/PARTICLE_AGE/4*multiplier,0,
						 -(particle->age<<SP_ACCURACY)/PARTICLE_AGE/4*multiplier,
							(particle->age<<SP_ACCURACY)/PARTICLE_AGE/4*multiplier,0,
							spGetHSV(particle->h,particle->s,particle->v));
		memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);
		particle=particle->next;
	}
	spSetAlphaTest(1);
		
	Sint32 r = 28<< SP_ACCURACY-5;
	
	//selection
	draw_particle(posx[0],posy[0],r,(game_counter)%PARTICLE_SPEED,getSmallParticle());
	draw_particle(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED/24)%PARTICLE_SPEED,getMiddleParticle());
	draw_particle(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED/10)%PARTICLE_SPEED,getBigParticle());
		
	draw_particle(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED/4)%PARTICLE_SPEED,getSmallParticle());
	draw_particle(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED/4+PARTICLE_SPEED/24)%PARTICLE_SPEED,getMiddleParticle());
	draw_particle(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED/4+PARTICLE_SPEED/10)%PARTICLE_SPEED,getBigParticle());
		
	draw_particle(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED/2)%PARTICLE_SPEED,getSmallParticle());
	draw_particle(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED/2+PARTICLE_SPEED/24)%PARTICLE_SPEED,getMiddleParticle());
	draw_particle(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED/2+PARTICLE_SPEED/10)%PARTICLE_SPEED,getBigParticle());
		
	draw_particle(posx[0],posy[0],r,(game_counter+3*PARTICLE_SPEED/4)%PARTICLE_SPEED,getSmallParticle());
	draw_particle(posx[0],posy[0],r,(game_counter+3*PARTICLE_SPEED/4+PARTICLE_SPEED/24)%PARTICLE_SPEED,getMiddleParticle());
	draw_particle(posx[0],posy[0],r,(game_counter+3*PARTICLE_SPEED/4+PARTICLE_SPEED/10)%PARTICLE_SPEED,getBigParticle());

	if (settings_get_control() == 1)
	{
		spBlit3D(spCos(-(pointposx[(	TIMEOUT-1+pointstart+2*TIMEOUT-TIMEOUT/4)%(2*TIMEOUT)]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,pointposy[(	TIMEOUT-1+pointstart+2*TIMEOUT-TIMEOUT/4)%(2*TIMEOUT)],spSin((pointposx[(	TIMEOUT-1+pointstart+2*TIMEOUT-TIMEOUT/4)%(2*TIMEOUT)]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,getMiddleParticle());
		spBlit3D(spCos(-(pointposx[(	TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,pointposy[(	TIMEOUT-1+pointstart)%(2*TIMEOUT)],spSin((pointposx[(	TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,getMiddleParticle());
		spBlit3D(spCos(-(pointposx[(	TIMEOUT-1+pointstart+TIMEOUT/4)%(2*TIMEOUT)]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,pointposy[(	TIMEOUT-1+pointstart+TIMEOUT/4)%(2*TIMEOUT)],spSin((pointposx[(	TIMEOUT-1+pointstart+TIMEOUT/4)%(2*TIMEOUT)]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,getMiddleParticle());

		draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
		draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

		draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
		draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

		draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
		draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

		draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
		draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
	}
	else
	if (settings_get_control() == 2)
	{
		switch (choose_one)
		{
			case 1: //Up
				draw_particle2(posx[0],posy[0]+(4<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]+(4<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0],posy[0]+(4<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]+(4<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0],posy[0]+(4<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]+(4<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0],posy[0]+(4<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]+(4<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;

			case 8: //Up-Left
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;
			case 2: //Up-Right
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]+(2<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;

			case 7: //Left
				draw_particle2(posx[0]-(2<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(2<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]-(2<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(2<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]-(2<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(2<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]-(2<<SP_ACCURACY),posy[0],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(2<<SP_ACCURACY),posy[0],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;
			case 0: //Middle
				draw_particle2(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0],posy[0],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;
			case 3: //Right
				draw_particle2(posx[0]+(2<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(2<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]+(2<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(2<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]+(2<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(2<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]+(2<<SP_ACCURACY),posy[0],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(2<<SP_ACCURACY),posy[0],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;
				
			case 6: //Down-Left
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;
			case 4: //Down-Right
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0]-(2<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;
		
			case 5: //Down
				draw_particle2(posx[0],posy[0]-(4<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]-(4<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0],posy[0]-(4<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]-(4<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0],posy[0]-(4<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]-(4<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				draw_particle2(posx[0],posy[0]-(4<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]-(4<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;
		}
	}
	else
	if (settings_get_control() == 0)
	{
		/*spRotate(0,+1<<SP_ACCURACY,0, (posx[0]>>SP_HALF_ACCURACY+1)*(SP_PI>>SP_HALF_ACCURACY+2)-SP_PI/8);
		spTranslate(0,posy[0],23<<SP_ACCURACY-2);
		SDL_Surface* surface = spFontGetLetter(font,'A')->surface;
		spBindTexture(surface);
		spQuadTex3D(-6<<SP_ACCURACY-3,-6<<SP_ACCURACY-3,0,0,surface->h-SP_FONT_EXTRASPACE-1,
								+6<<SP_ACCURACY-3,-6<<SP_ACCURACY-3,0,surface->w-SP_FONT_EXTRASPACE-1,surface->h-SP_FONT_EXTRASPACE-1,
								+6<<SP_ACCURACY-3,+6<<SP_ACCURACY-3,0,surface->w-SP_FONT_EXTRASPACE-1,0,
								-6<<SP_ACCURACY-3,+6<<SP_ACCURACY-3,0,0,0,65535);
		spTranslate(0,-posy[0],-23<<SP_ACCURACY-2);
		spRotate(0,-1<<SP_ACCURACY,0, (posx[0]>>SP_HALF_ACCURACY+1)*(SP_PI>>SP_HALF_ACCURACY+2)-SP_PI/8);*/
		
		int x = (20-(posx[0]>>SP_ACCURACY)) & 15;
		int y = 3+(posy[0]>>SP_ACCURACY+1);		
		spRotate(0,+1<<SP_ACCURACY,0, (posx[0]>>SP_HALF_ACCURACY+1)*(SP_PI>>SP_HALF_ACCURACY+2));
		if (choose_one!=1 && stone[y][(x+1) & 15].new == 0 && stone[y][(x+1) & 15].falling == 0 && stone[y][(x+1) & 15].type >= 0)
			spBlit3D(-7<<SP_ACCURACY-2,posy[0],23<<SP_ACCURACY-2,spFontGetButton(small_font,'A')->surface);
		if (choose_one!=2 && stone[y][(x+15) & 15].new == 0 && stone[y][(x+15) & 15].falling == 0 && stone[y][(x+15) & 15].type >= 0)
			spBlit3D(+7<<SP_ACCURACY-2,posy[0],23<<SP_ACCURACY-2,spFontGetButton(small_font,'B')->surface);
		if (choose_one!=3 && y>0 && stone[y-1][x].new == 0 && stone[y-1][x].falling == 0 && stone[y-1][x].type >= 0)
			spBlit3D(0,posy[0]-(7<<SP_ACCURACY-2),23<<SP_ACCURACY-2,spFontGetButton(small_font,'X')->surface);
		if (choose_one!=4 && y<6 && stone[y+1][x].new == 0 && stone[y+1][x].falling == 0 && stone[y+1][x].type >= 0)
			spBlit3D(0,posy[0]+(7<<SP_ACCURACY-2),23<<SP_ACCURACY-2,spFontGetButton(small_font,'Y')->surface);
		spRotate(0,-1<<SP_ACCURACY,0, (posx[0]>>SP_HALF_ACCURACY+1)*(SP_PI>>SP_HALF_ACCURACY+2));
		
		switch (choose_one)
		{
			case 1: 
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]-(1<<SP_ACCURACY),posy[0],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;
			case 2: 
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0]+(1<<SP_ACCURACY),posy[0],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;
			case 3: 
				draw_particle2(posx[0],posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

				draw_particle2(posx[0],posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

				draw_particle2(posx[0],posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]-(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

				draw_particle2(posx[0],posy[0]-(2<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]-(2<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;
			case 4: 
				draw_particle2(posx[0],posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

				draw_particle2(posx[0],posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

				draw_particle2(posx[0],posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]+(2<<SP_ACCURACY),r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

				draw_particle2(posx[0],posy[0]+(2<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
				draw_particle2(posx[0],posy[0]+(2<<SP_ACCURACY),r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());
				break;
		}
	}	
	char buffer[256];
	
	spSetZSet(0);
	spSetZTest(0);
	
	//Countdown
	if (countdown>0 && countdown<4000)
	{
		int factor = (countdown % 1000)+500;
		SDL_Surface* surface = NULL;
		switch (countdown / 1000)
		{
			case 3: surface = spFontGetLetter(countdown_font,'3')->surface; break;
			case 2: surface = spFontGetLetter(countdown_font,'2')->surface; break;
			case 1: surface = spFontGetLetter(countdown_font,'1')->surface; break;
			default: surface = spFontGetLetter(countdown_font,'0')->surface; break;
		}
		spBindTexture(surface);
		spQuad_tex(screen->w/2-factor*screen->w/5000,screen->h/2+factor*screen->w/3000,-1,0,surface->h-SP_FONT_EXTRASPACE-1,
							 screen->w/2+factor*screen->w/5000,screen->h/2+factor*screen->w/3000,-1,surface->w-SP_FONT_EXTRASPACE-1,surface->h-SP_FONT_EXTRASPACE-1,
							 screen->w/2+factor*screen->w/5000,screen->h/2-factor*screen->w/3000,-1,surface->w-SP_FONT_EXTRASPACE-1,0,
							 screen->w/2-factor*screen->w/5000,screen->h/2-factor*screen->w/3000,-1,0,0,65535);
	}
	
	//HUD on the left side

	plettering lettering = get_first_lettering();
	while (lettering)
	{
		if (lettering->text)
			spFontDrawMiddle(engineWindowX/7,lettering->y,-1,lettering->text,lettering->font);
		lettering = lettering->next;
	}
	
	//pointVis
	
	pPointVis mom = first_pointVis;
	while (mom)
	{
		int y = mom->age*engineWindowY / (POINTAGE-50);
		char buffer[32];
		sprintf(buffer,"+%i",mom->points);
		spFontDrawMiddle(engineWindowX/2,engineWindowY/2-y,-1,buffer,font);
		mom = mom->next;
	}
	//HUD on the right side
	spFontDrawMiddle(6*engineWindowX/7,1*engineWindowY/16,-1,spGetTranslationFromCaption(translation,"Game Mode:"),small_font);
	if (settings_get_mode() == 1)
		sprintf(buffer,"%s",spGetTranslationFromCaption(translation,"Survival"));
	else
	if (settings_get_mode() == 0)
		sprintf(buffer,"%s",spGetTranslationFromCaption(translation,"Points"));
	else
		sprintf(buffer,"%s",spGetTranslationFromCaption(translation,"Race"));
	spFontDrawMiddle(6*engineWindowX/7,2*engineWindowY/16,-1,buffer,font);
	if (settings_get_mode() == 1)
	{
		spFontDrawMiddle(6*engineWindowX/7,4*engineWindowY/16,-1,spGetTranslationFromCaption(translation,"Time survived"),small_font);
		sprintf(buffer,"%i.%i %s",realTime/1000,(realTime/100)%10,spGetTranslationFromCaption(translation,"Sec"));
		spFontDrawMiddle(6*engineWindowX/7,5*engineWindowY/16,-1,buffer,font);
	}
	else
	{
		spFontDrawMiddle(6*engineWindowX/7,4*engineWindowY/16,-1,spGetTranslationFromCaption(translation,"Points"),small_font);
		sprintf(buffer,"%i",points);
		spFontDrawMiddle(6*engineWindowX/7,5*engineWindowY/16,-1,buffer,font);
		if (settings_get_mode() == 0)
		{
			sprintf(buffer,"%i.%i %s",(120000-realTime)/1000,((120000-realTime)/100)%10,spGetTranslationFromCaption(translation,"Sec"));
			spFontDrawMiddle(6*engineWindowX/7,8*engineWindowY/16,-1,buffer,font);
		}
	}

	if (settings_get_mode() == 2)
	{
		spFontDrawMiddle(6*engineWindowX/7,7*engineWindowY/16,-1,spGetTranslationFromCaption(translation,"Time elapsed"),small_font);
		sprintf(buffer,"%i.%i %s",realTime/1000,(realTime/100)%10,spGetTranslationFromCaption(translation,"Sec"));
		spFontDrawMiddle(6*engineWindowX/7,8*engineWindowY/16,-1,buffer,font);
	}
	else
		spBlitSurfacePart(6*engineWindowX/7,14*engineWindowY/32,-1,getTimeSurface(),0,0,gameTime*getTimeSurface()->w/START_TIME,getTimeSurface()->h);

	spFontDrawMiddle(6*engineWindowX/7,20*engineWindowY/32,-1,"[S]",small_font);
	spFontDrawMiddle(6*engineWindowX/7,23*engineWindowY/32,-1,spGetTranslationFromCaption(translation,"Pause"),small_font);
	spFontDrawMiddle(6*engineWindowX/7,26*engineWindowY/32,-1,"[E]",small_font);
	spFontDrawMiddle(6*engineWindowX/7,29*engineWindowY/32,-1,spGetTranslationFromCaption(translation,"Back to Menu"),small_font);


	//sprintf(buffer,"fps %i",spGetFPS());
	//spFontDraw(2,engineWindowY-small_font->maxheight,-1,buffer,small_font);

	//help text
	if (countdown == 4000)
	{
		draw_filled_border(7*engineWindowX/32,1*engineWindowY/20,25*engineWindowX/32,19*engineWindowY/20,BACKGROUND_COLOR);
		draw_border			 (7*engineWindowX/32,1*engineWindowY/20,25*engineWindowX/32,19*engineWindowY/20,65535);
		
		spFontDrawMiddle(engineWindowX/2,1*engineWindowY/12,-1,"How to play the",font);
		switch (settings_get_mode())
		{
			case 0:
				spFontDrawMiddle(engineWindowX/2, 2*engineWindowY/12,-1,"\"Points Mode\":",font);
				spFontDrawMiddle(engineWindowX/2, 4*engineWindowY/12,-1,"Get as much",font);
				spFontDrawMiddle(engineWindowX/2, 5*engineWindowY/12,-1,"points as",font);
				spFontDrawMiddle(engineWindowX/2, 6*engineWindowY/12,-1,"possible in",font);
				spFontDrawMiddle(engineWindowX/2, 7*engineWindowY/12,-1,"120 Seconds",font);
				break;
			case 1:
				spFontDrawMiddle(engineWindowX/2, 2*engineWindowY/12,-1,"\"Survival Mode\":",font);
				spFontDrawMiddle(engineWindowX/2, 4*engineWindowY/12,-1,"You have a",font);
				spFontDrawMiddle(engineWindowX/2, 5*engineWindowY/12,-1,"limited life-span",font);
				spFontDrawMiddle(engineWindowX/2, 6*engineWindowY/12,-1,"decreasing faster",font);
				spFontDrawMiddle(engineWindowX/2, 7*engineWindowY/12,-1,"every 20s, fill it",font);
				spFontDrawMiddle(engineWindowX/2, 8*engineWindowY/12,-1,"by smashing blocks",font);
				break;
			case 2:
				spFontDrawMiddle(engineWindowX/2, 2*engineWindowY/12,-1,"\"Race Mode\":",font);
				spFontDrawMiddle(engineWindowX/2, 4*engineWindowY/12,-1,"Get 100.000",font);
				spFontDrawMiddle(engineWindowX/2, 5*engineWindowY/12,-1,"points as",font);
				spFontDrawMiddle(engineWindowX/2, 6*engineWindowY/12,-1,"fast as",font);
				spFontDrawMiddle(engineWindowX/2, 7*engineWindowY/12,-1,"possible",font);
				break;
		}
		spFontDrawMiddle(engineWindowX/2,10*engineWindowY/12,-1,"Press [A],[B],[X] or [Y]",small_font);
	}
	
	//insert name
	if (insert_name)
	{
		int trophy;
		switch (settings_get_mode())
		{
			case 0: //Points
				trophy = get_highscore_trophy(settings_get_mode(),settings_get_color(),points);
				break;
			case 1: case 2: //Survival, Race
				trophy = get_highscore_trophy(settings_get_mode(),settings_get_color(),realTime/100);
				break;
		}
		draw_filled_border(5*engineWindowX/20,1*engineWindowY/20,15*engineWindowX/20,19*engineWindowY/20,BACKGROUND_COLOR);
		draw_border       (5*engineWindowX/20,1*engineWindowY/20,15*engineWindowX/20,19*engineWindowY/20,65535);
		int movement = 0;
		if (trophy < 4)
		{
			spBindTexture( trophy_texture[trophy] );
			spQuad_tex(23*engineWindowX/40, 3*engineWindowY/40,0,0,0,
			           23*engineWindowX/40,13*engineWindowY/40,0,0,trophy_texture[trophy]->h-1,
			           29*engineWindowX/40,13*engineWindowY/40,0,trophy_texture[trophy]->w-1,trophy_texture[trophy]->h-1,
			           29*engineWindowX/40, 3*engineWindowY/40,0,trophy_texture[trophy]->w-1,0,65535);
			movement = 3*engineWindowX/40;
		}
		
		spFontDrawMiddle(engineWindowX/2-movement, 1*engineWindowY/12,-1,spGetTranslationFromCaption(translation,"You got"),font);
		switch (settings_get_mode())
		{
			case 0: sprintf(buffer,"%i %s",points,spGetTranslationFromCaption(translation,"Points")); break;
			case 1: case 2: sprintf(buffer,"%i.%i %s",realTime/1000,(realTime/100)%10,spGetTranslationFromCaption(translation,"Sec")); break;
		}
		spFontDrawMiddle(engineWindowX/2-movement, 2*engineWindowY/12,-1,buffer,font);
		sprintf(buffer,"(%s %i)",spGetTranslationFromCaption(translation,"Rank"),insert_name);
		spFontDrawMiddle(engineWindowX/2-movement,12*engineWindowY/48,-1,buffer,small_font);
		switch (settings_get_mode())
		{
			case 0: //Points
				sprintf(buffer,"%i %s",get_next_highscore_trophy(settings_get_mode(),settings_get_color(),points),spGetTranslationFromCaption(translation,"Points"));
				break;
			case 1: case 2://Survival, Race
				sprintf(buffer,"%i.%i %s",get_next_highscore_trophy(settings_get_mode(),settings_get_color(),realTime/100)/10,get_next_highscore_trophy(settings_get_mode(),settings_get_color(),realTime/100)%10,spGetTranslationFromCaption(translation,"Sec"));
				break;
		}
		switch (trophy)
		{
			case 3:
				spFontDrawMiddle(engineWindowX/2, 8*engineWindowY/24,-1,spGetTranslationFromCaption(translation,"& the bronze trophy!"),middle_font);
				spFontDrawMiddle(engineWindowX/2,19*engineWindowY/48,-1,spGetTranslationFromCaption(translation,"next:"),small_font);
				spFontDrawMiddle(engineWindowX/2,21*engineWindowY/48,-1,buffer,small_font);
				break;
			case 2:
				spFontDrawMiddle(engineWindowX/2, 8*engineWindowY/24,-1,spGetTranslationFromCaption(translation,"& the silver trophy!"),middle_font);
				spFontDrawMiddle(engineWindowX/2,19*engineWindowY/48,-1,spGetTranslationFromCaption(translation,"next:"),small_font);
				spFontDrawMiddle(engineWindowX/2,21*engineWindowY/48,-1,buffer,small_font);
				break;
			case 1:
				spFontDrawMiddle(engineWindowX/2, 8*engineWindowY/24,-1,spGetTranslationFromCaption(translation,"& the gold trophy!"),middle_font);
				spFontDrawMiddle(engineWindowX/2,19*engineWindowY/48,-1,spGetTranslationFromCaption(translation,"next:"),small_font);
				spFontDrawMiddle(engineWindowX/2,21*engineWindowY/48,-1,buffer,small_font);
				break;
			case 0:
				spFontDrawMiddle(engineWindowX/2, 8*engineWindowY/24,-1,spGetTranslationFromCaption(translation,"& the green trophy!"),middle_font);
				spFontDrawMiddle(engineWindowX/2,20*engineWindowY/48,-1,spGetTranslationFromCaption(translation,"(better than the maker)"),small_font);
				break;
		}
		spFontDrawMiddle(engineWindowX/2,12*engineWindowY/24,-1,spGetTranslationFromCaption(translation,"Enter your name"),font);
		spFontDrawMiddle(engineWindowX/2,14*engineWindowY/24,-1,spGetTranslationFromCaption(translation,"with the D-pad"),font);
		
		int i;
		for (i = 0; i < 3; i++)
		{
			SDL_Surface* surface = spFontGetLetter(highscore_font,myhighscore_name[i])->surface;
			spBindTexture(surface);
			int addx	= (i-1)*2*spGetSizeFactor()*24>>SP_ACCURACY;
			int sizex = spGetSizeFactor()*24>>SP_ACCURACY;
			int sizey = spGetSizeFactor()*32>>SP_ACCURACY;
			if (i!=choosen_letter)
			{
				sizex = sizex*3/4;
				sizey = sizey*3/4;
			}
			spQuad_tex(addx+engineWindowX/2-sizex,15*engineWindowY/20+sizey,-1,0,surface->h-SP_FONT_EXTRASPACE-1,
								 addx+engineWindowX/2+sizex,15*engineWindowY/20+sizey,-1,surface->w-SP_FONT_EXTRASPACE-1,surface->h-SP_FONT_EXTRASPACE-1,
								 addx+engineWindowX/2+sizex,15*engineWindowY/20-sizey,-1,surface->w-SP_FONT_EXTRASPACE-1,0,
								 addx+engineWindowX/2-sizex,15*engineWindowY/20-sizey,-1,0,0,65535);
		}
			 
		spFontDrawMiddle(engineWindowX/2, 41*engineWindowY/48,-1,spGetTranslationFromCaption(translation,"and press [S]"),font);		
	}
	

	

	draw_music();
	draw_c4a();
		
	spFlip();
}

int control_timeout;

int calc_game(Uint32 steps)
{
	calc_music(steps);
	calc_c4a(steps);
	PspInput engineInput = spGetInput();
	if (insert_name)
	{
		int i;
		for (i = 0; i < steps; i++)
		{
			if (control_timeout)
				control_timeout--;
			if (engineInput->axis[0] < 0 && (control_timeout<=0))
			{
				choosen_letter = (choosen_letter+2)%3;
				control_timeout = 300;
			}
			if (engineInput->axis[0] > 0 && (control_timeout<=0))
			{
				choosen_letter = (choosen_letter+1)%3;
				control_timeout = 300;
			}
			if (engineInput->axis[1] > 0 && (control_timeout<=0))
			{
				myhighscore_name[choosen_letter]--;
				if (myhighscore_name[choosen_letter] < 'A')
					myhighscore_name[choosen_letter] = 'Z';
				control_timeout = 200;
			}
			if (engineInput->axis[1] < 0 && (control_timeout<=0))
			{
				myhighscore_name[choosen_letter]++;
				if (myhighscore_name[choosen_letter] > 'Z')
					myhighscore_name[choosen_letter] = 'A';
				control_timeout = 200;
			}
			if (engineInput->button[SP_BUTTON_START])
			{
				settings_set_highscore_name(myhighscore_name);
				settings_save();
				switch (settings_get_mode())
				{
					case 0: insert_highscore(settings_get_mode(),settings_get_color(),myhighscore_name,points); break;
					case 1: insert_highscore(settings_get_mode(),settings_get_color(),myhighscore_name,realTime/100); break;
					case 2: insert_highscore(settings_get_mode(),settings_get_color(),myhighscore_name,realTime/100); break;
				}
				highscore_save();
				engineInput->button[SP_BUTTON_START]=0;
				return 1;
			}
		}
		return 0;
	}
	if (engineInput->button[SP_BUTTON_START])
	{
		pause=!pause;
		engineInput->button[SP_BUTTON_START]=0;
	}
	
	if (pause)
		return 0;
	if (countdown == 4000)
	{
		if (engineInput->button[SP_BUTTON_B] || engineInput->button[SP_BUTTON_A] ||
				engineInput->button[SP_BUTTON_X] || engineInput->button[SP_BUTTON_Y])
		{
			engineInput->button[SP_BUTTON_A] = 0;
			engineInput->button[SP_BUTTON_B] = 0;
			engineInput->button[SP_BUTTON_X] = 0;
			engineInput->button[SP_BUTTON_Y] = 0;
			countdown = 3999;
		}
		return 0;
	}
	if (countdown>0)
	{
		countdown-=steps;
		return 0;
	}
	calc_pointVis(steps);
	game_counter+=steps;
	int i;
	for (i=0;i<steps;i++)
	{
		realTime++;
		if (settings_get_mode() == 1)
		{
			if (realTime%20000 == 0)
			{
				timeStep++; //= 2*timeStep-timeStep/2;
				add_lettering("Faster!",settings_get_font());
			}
		}
		gameTime-=timeStep;
		if ((settings_get_mode()!=2 && gameTime<=0) || (settings_get_mode()==2 && points>=POINTS_GOAL))
		{
			move_sound_off();
			rotating_sound_off();
			switch (settings_get_mode())
			{
				case 0:
					if (points > get_highscore(0,settings_get_color(),2))
						insert_name = 3;
					if (points > get_highscore(0,settings_get_color(),1))
						insert_name = 2;
					if (points > get_highscore(0,settings_get_color(),0))
						insert_name = 1;
					if (insert_name)
						init_one_score_commit(0,points);
					break;
				case 1:
					if (realTime/100 > get_highscore(1,settings_get_color(),2))
						insert_name = 3;
					if (realTime/100 > get_highscore(1,settings_get_color(),1))
						insert_name = 2;
					if (realTime/100 > get_highscore(1,settings_get_color(),0))
						insert_name = 1;
					if (insert_name)
						init_one_score_commit(1,realTime/100);
					break;
				case 2:
					if (realTime/100 < get_highscore(2,settings_get_color(),2))
						insert_name = 3;
					if (realTime/100 < get_highscore(2,settings_get_color(),1))
						insert_name = 2;
					if (realTime/100 < get_highscore(2,settings_get_color(),0))
						insert_name = 1;
					if (insert_name)
						init_one_score_commit(2,realTime/100);
				break;
			}
			if (insert_name == 0)
				return 1;
			return 0;
		}

		test_and_set_chain();
		step_changes();
		step_particles();
		make_win_situations_invalid();
		//new stones
		int a,y;
		for (a=0;a<16;a++)
			for (y=0;y<7;y++)
				if (stone[y][a].new>0)
					stone[y][a].new--;
		//falling stones
		for (a=0;a<16;a++)
			for (y=0;y<7;y++)
				if (stone[y][a].type<0)
				{
					int y2;
					for (y2=y+1;y2<7;y2++)
						if (stone[y2][a].type>=0 &&
								stone[y2][a].falling==0)
							stone[y2][a].falling=FALL_TIME;
					if (y==6)
					{
						if (settings_get_color())
						{
							if (rand()%LIKELYHOOD < LIKELYHOOD-1)
								stone[y][a].type=rand()%6;
							else
								stone[y][a].type=rand()%3+6;
						}
						else
						{
							if (rand()%LIKELYHOOD < LIKELYHOOD-1)
								stone[y][a].type=rand()%4;
							else
								stone[y][a].type=rand()%3+6;
						}
						stone[y][a].h=get_type_color_h(stone[y][a].type,w);
						stone[y][a].s=get_type_color_s(stone[y][a].type,w);
						stone[y][a].v=get_type_color_v(stone[y][a].type,w);
						stone[y][a].falling=0;
						stone[y][a].new=NEW_TIME;
					}
				}
		for (a=0;a<16;a++)
			for (y=1;y<7;y++)
			if (stone[y][a].falling)
			{
				stone[y][a].falling--;
				if (stone[y][a].falling<=0)
				{					
					stone[y-1][a]=stone[y	][a];
					stone[y-1][a].falling=0;
					if (y==6)
					{
						if (settings_get_color())
						{
							if (rand()%LIKELYHOOD < LIKELYHOOD-1)
								stone[y][a].type=rand()%6;
							else
								stone[y][a].type=rand()%3+6;
						}
						else
						{
							if (rand()%LIKELYHOOD < LIKELYHOOD-1)
								stone[y][a].type=rand()%4;
							else
								stone[y][a].type=rand()%3+6;
						}
						stone[y][a].h=get_type_color_h(stone[y][a].type,w);
						stone[y][a].s=get_type_color_s(stone[y][a].type,w);
						stone[y][a].v=get_type_color_v(stone[y][a].type,w);
						stone[y][a].falling=0;
						stone[y][a].new=NEW_TIME;
					}
					else 
						stone[y][a].type=-1;
				}
			}
		if (timeout>0)
		{
			switch (direction)
			{
				case 0:
					posx[0]-=(1<<SP_ACCURACY)/TIMEOUT;
					if (posx[0]<0)
						posx[0]+=16<<SP_ACCURACY;
					break;
				case 1:
					posx[0]+=(1<<SP_ACCURACY)/TIMEOUT;
					if (posx[0]>=(16<<SP_ACCURACY))
						posx[0]-=16<<SP_ACCURACY;
					break;
				case 2:
					posy[0]-=(1<<SP_ACCURACY)/(TIMEOUT/2);
					if (posy[0]<(-6<<SP_ACCURACY))
						posy[0]=(-6<<SP_ACCURACY);
					break;
				case 3:
					posy[0]+=(1<<SP_ACCURACY)/(TIMEOUT/2);
					if (posy[0]>(6<<SP_ACCURACY))
						posy[0]=(6<<SP_ACCURACY);
					break;
			}
			timeout--;
			pointstart--;
			if (pointstart<0)
				pointstart=2*TIMEOUT-1;
			pointposx[pointstart]=posx[0];
			pointposy[pointstart]=posy[0];
			if (timeout==TIMEOUT/2)
			{
				posx[2]=posx[1];
				posy[2]=posy[1];
				posx[1]=posx[3];
				posy[1]=posy[3];				
			}
			if (timeout<=0)
			{
				posx[0]=(posx[0]>>SP_ACCURACY)<<SP_ACCURACY;
				posy[0]=(posy[0]>>SP_ACCURACY)<<SP_ACCURACY;
				posx[3]=posx[0];
				posy[3]=posy[0];
				is_change=0;
			}
		}
		else
		if (settings_get_control()==1)
		{
			if (engineInput->axis[0]<0)
			{
				if (direction==1)
				{
					Sint32 temp=posx[0];
					posx[0]=posx[2];
					posx[2]=temp;
					temp=posy[0];
					posy[0]=posy[2];
					posy[2]=temp;
					posx[3]=posx[0];
					posy[3]=posy[0];
					int i;
					for (i=0;i<TIMEOUT;i++)
					{
						temp=pointposx[(pointstart+i)%(2*TIMEOUT)];
						pointposx[(pointstart+i)%(2*TIMEOUT)]=pointposx[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)];
						pointposx[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)]=temp;
						temp=pointposy[(pointstart+i)%(2*TIMEOUT)];
						pointposy[(pointstart+i)%(2*TIMEOUT)]=pointposy[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)];
						pointposy[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)]=temp;
					}
					is_change=posx[2]-posx[3]>>SP_ACCURACY;
					if (is_change>=14)
						is_change-=16;
					if (is_change<=-14)
						is_change+=16;
				}
				direction=0;
				timeout=TIMEOUT;
				rotating_sound_on();
			}
			else
			if (engineInput->axis[0]>0)
			{
				if (direction==0)
				{
					Sint32 temp=posx[0];
					posx[0]=posx[2];
					posx[2]=temp;
					temp=posy[0];
					posy[0]=posy[2];
					posy[2]=temp;
					posx[3]=posx[0];
					posy[3]=posy[0];
					int i;
					for (i=0;i<TIMEOUT;i++)
					{
						temp=pointposx[(pointstart+i)%(2*TIMEOUT)];
						pointposx[(pointstart+i)%(2*TIMEOUT)]=pointposx[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)];
						pointposx[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)]=temp;
						temp=pointposy[(pointstart+i)%(2*TIMEOUT)];
						pointposy[(pointstart+i)%(2*TIMEOUT)]=pointposy[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)];
						pointposy[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)]=temp;
					}
					is_change=posx[2]-posx[3]>>SP_ACCURACY;
					if (is_change>=14)
						is_change-=16;
					if (is_change<=-14)
						is_change+=16;
				}
				direction=1;
				timeout=TIMEOUT+1;
				rotating_sound_on();
			}
			else
				rotating_sound_off();
			if (engineInput->axis[1]>0 && (((posy[0]>>SP_ACCURACY)>-6 && direction!=3) || (direction==3 && (posy[2]>>SP_ACCURACY)>-6)))
			{
				if (direction==3 && (posy[2]>>SP_ACCURACY)>-6)
				{
					Sint32 temp=posx[0];
					posx[0]=posx[2];
					posx[2]=temp;
					temp=posy[0];
					posy[0]=posy[2];
					posy[2]=temp;
					posx[3]=posx[0];
					posy[3]=posy[0];
					int i;
					for (i=0;i<TIMEOUT;i++)
					{
						temp=pointposx[(pointstart+i)%(2*TIMEOUT)];
						pointposx[(pointstart+i)%(2*TIMEOUT)]=pointposx[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)];
						pointposx[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)]=temp;
						temp=pointposy[(pointstart+i)%(2*TIMEOUT)];
						pointposy[(pointstart+i)%(2*TIMEOUT)]=pointposy[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)];
						pointposy[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)]=temp;
					}
					is_change=posx[2]-posx[3]>>SP_ACCURACY;
					if (is_change>=14)
						is_change-=16;
					if (is_change<=-14)
						is_change+=16;
				}
				direction=2;
				timeout=TIMEOUT;
				move_sound_on();
			}
			else
			if (engineInput->axis[1]<0 && (((posy[0]>>SP_ACCURACY)< 6 && direction!=2) || (direction==2 && (posy[2]>>SP_ACCURACY)< 6)))
			{
				if (direction==2 && (posy[2]>>SP_ACCURACY)< 6)
				{
					Sint32 temp=posx[0];
					posx[0]=posx[2];
					posx[2]=temp;
					temp=posy[0];
					posy[0]=posy[2];
					posy[2]=temp;
					posx[3]=posx[0];
					posy[3]=posy[0];
					int i;
					for (i=0;i<TIMEOUT;i++)
					{
						temp=pointposx[(pointstart+i)%(2*TIMEOUT)];
						pointposx[(pointstart+i)%(2*TIMEOUT)]=pointposx[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)];
						pointposx[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)]=temp;
						temp=pointposy[(pointstart+i)%(2*TIMEOUT)];
						pointposy[(pointstart+i)%(2*TIMEOUT)]=pointposy[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)];
						pointposy[(pointstart+2*TIMEOUT-1-i)%(2*TIMEOUT)]=temp;
					}
					is_change=posx[2]-posx[3]>>SP_ACCURACY;
					if (is_change>=14)
						is_change-=16;
					if (is_change<=-14)
						is_change+=16;
				}
				direction=3;
				timeout=TIMEOUT+1;
				move_sound_on();
			}
			else
				move_sound_off();
			if (engineInput->button[SP_BUTTON_B] || engineInput->button[SP_BUTTON_A] ||
					engineInput->button[SP_BUTTON_X] || engineInput->button[SP_BUTTON_Y])
			{
				if (is_in_change((20-(posx[0]>>SP_ACCURACY))%16,3+(posy[0]>>SP_ACCURACY+1))==NULL &&
						is_in_change((20-(posx[2]>>SP_ACCURACY))%16,3+(posy[2]>>SP_ACCURACY+1))==NULL &&
						stone[3+(posy[0]>>SP_ACCURACY+1)][(20-(posx[0]>>SP_ACCURACY))%16].new==0 &&
						stone[3+(posy[2]>>SP_ACCURACY+1)][(20-(posx[2]>>SP_ACCURACY))%16].new==0 &&
						stone[3+(posy[0]>>SP_ACCURACY+1)][(20-(posx[0]>>SP_ACCURACY))%16].falling==0 &&
						stone[3+(posy[2]>>SP_ACCURACY+1)][(20-(posx[2]>>SP_ACCURACY))%16].falling==0 &&
						stone[3+(posy[0]>>SP_ACCURACY+1)][(20-(posx[0]>>SP_ACCURACY))%16].type>=0 &&
						stone[3+(posy[2]>>SP_ACCURACY+1)][(20-(posx[2]>>SP_ACCURACY))%16].type>=0)
				{
					play_switch();
					initate_change((20-(posx[0]>>SP_ACCURACY))%16,3+(posy[0]>>SP_ACCURACY+1),(20-(posx[2]>>SP_ACCURACY))%16,3+(posy[2]>>SP_ACCURACY+1));
				}
			}
		}
		else //new, better control
		if (settings_get_control() == 0)
		{
			if (!choose_one && engineInput->axis[0]<0)
			{
				direction=0;
				timeout=TIMEOUT;
				rotating_sound_on();
			}
			else
			if (!choose_one && engineInput->axis[0]>0)
			{
				direction=1;
				timeout=TIMEOUT+1;
				rotating_sound_on();
			}
			else
				rotating_sound_off();
			if (!choose_one && engineInput->axis[1]>0 && (posy[0]>>SP_ACCURACY)>-6)
			{
				direction=2;
				timeout=TIMEOUT;
				move_sound_on();
			}
			else
			if (!choose_one && engineInput->axis[1]<0 && (posy[0]>>SP_ACCURACY)< 6)
			{
				direction=3;
				timeout=TIMEOUT+1;
				move_sound_on();
			}
			else
				move_sound_off();
			if (engineInput->button[SP_BUTTON_LEFT])
			{
				engineInput->button[SP_BUTTON_LEFT] = 0;
				int x = (20-(posx[0]>>SP_ACCURACY)) & 15;
				int y = 3+(posy[0]>>SP_ACCURACY+1);
				if (!choose_one && stone[y][(x+1) & 15].new == 0 && stone[y][(x+1) & 15].falling == 0 && stone[y][(x+1) & 15].type >= 0)
					choose_one = 1;
				else
				if (choose_one == 1)
					choose_one = 0;
				else
				if (stone[y][(x+15) & 1].new == 0 && stone[y][(x+1) & 15].falling == 0 && stone[y][(x+1) & 15].type >= 0)
				{
					play_switch();
					switch (choose_one)
					{
						case 2: initate_change((x+1) & 15,y,(x+15) & 15,y); break;
						case 3: initate_change((x+1) & 15,y,x,y-1); break;
						case 4: initate_change((x+1) & 15,y,x,y+1); break;
					}
					choose_one = 0;
				}
			}
			if (engineInput->button[SP_BUTTON_RIGHT])
			{
				engineInput->button[SP_BUTTON_RIGHT] = 0;
				int x = (20-(posx[0]>>SP_ACCURACY)) & 15;
				int y = 3+(posy[0]>>SP_ACCURACY+1);
				if (!choose_one && stone[y][(x+15) & 15].new == 0 && stone[y][(x+15) & 15].falling == 0 && stone[y][(x+15) & 15].type >= 0)
					choose_one = 2;
				else
				if (choose_one == 2)
					choose_one = 0;
				else
				if (stone[y][(x+15) & 15].new == 0 && stone[y][(x+15) & 15].falling == 0 && stone[y][(x+15) & 15].type >= 0)
				{
					play_switch();
					switch (choose_one)
					{
						case 1: initate_change((x+15) & 15,y,(x+1) & 15,y); break;
						case 3: initate_change((x+15) & 15,y,x,y-1); break;
						case 4: initate_change((x+15) & 15,y,x,y+1); break;
					}
					choose_one = 0;
				}
			}
			if (engineInput->button[SP_BUTTON_DOWN])
			{
				engineInput->button[SP_BUTTON_DOWN] = 0;
				int x = (20-(posx[0]>>SP_ACCURACY)) & 15;
				int y = 3+(posy[0]>>SP_ACCURACY+1);
				if (y>0 && !choose_one && stone[y-1][x].new == 0 && stone[y-1][x].falling == 0 && stone[y-1][x].type >= 0)
					choose_one = 3;
				else
				if (choose_one == 3)
					choose_one = 0;
				else
				if (y>0 && stone[y-1][x].new == 0 && stone[y-1][x].falling == 0 && stone[y-1][x].type >= 0)
				{
					play_switch();
					switch (choose_one)
					{
						case 1: initate_change(x,y-1,(x+1) & 15,y); break;
						case 2: initate_change(x,y-1,(x+15) & 15,y); break;
						case 4: initate_change(x,y-1,x,y+1); break;
					}
					choose_one = 0;
				}
			}			
			if (engineInput->button[SP_BUTTON_UP])
			{
				engineInput->button[SP_BUTTON_UP] = 0;
				int x = (20-(posx[0]>>SP_ACCURACY)) & 15;
				int y = 3+(posy[0]>>SP_ACCURACY+1);
				if (y<6 && !choose_one && stone[y+1][x].new == 0 && stone[y+1][x].falling == 0 && stone[y+1][x].type >= 0)
					choose_one = 4;
				else
				if (choose_one == 4)
					choose_one = 0;
				else
				if (y<6 && stone[y+1][x].new == 0 && stone[y+1][x].falling == 0 && stone[y+1][x].type >= 0)
				{
					play_switch();
					switch (choose_one)
					{
						case 1: initate_change(x,y+1,(x+1) & 15,y); break;
						case 2: initate_change(x,y+1,(x+15) & 15,y); break;
						case 3: initate_change(x,y+1,x,y-1); break;
					}
					choose_one = 0;
				}
			}
		}
		else //Fusion Power Control
		if (settings_get_control() == 2)
		{
			if (choose_one == -1)
			{
				if (control_timeout)
					control_timeout-=1;
				if (engineInput->axis[0] == 0 && engineInput->axis[1] == 0)
					control_timeout = 0;
				if (engineInput->axis[0]<0 && control_timeout<=0)
				{
					direction=0;
					timeout=TIMEOUT;
					rotating_sound_on();
				}
				else
				if (engineInput->axis[0]>0 && control_timeout<=0)
				{
					direction=1;
					timeout=TIMEOUT+1;
					rotating_sound_on();
				}
				else
					rotating_sound_off();
				if (engineInput->axis[1]>0 && (posy[0]>>SP_ACCURACY)>-6 && control_timeout<=0)
				{
					direction=2;
					timeout=TIMEOUT;
					move_sound_on();
				}
				else
				if (engineInput->axis[1]<0 && (posy[0]>>SP_ACCURACY)< 6 && control_timeout<=0)
				{
					direction=3;
					timeout=TIMEOUT+1;
					move_sound_on();
				}
				else
					move_sound_off();
			}
			if (engineInput->button[SP_BUTTON_A] || engineInput->button[SP_BUTTON_B] ||
					engineInput->button[SP_BUTTON_X] || engineInput->button[SP_BUTTON_Y])
			{
				rotating_sound_off();
				move_sound_off();
				if (engineInput->axis[0]==0 && engineInput->axis[1]<0 && (posy[0]>>SP_ACCURACY)<4)
					choose_one = 1;
				else
				if (engineInput->axis[0]>0 && engineInput->axis[1]<0 && (posy[0]>>SP_ACCURACY)<6)
					choose_one = 2;
				else
				if (engineInput->axis[0]>0 && engineInput->axis[1]==0)
					choose_one = 3;
				else
				if (engineInput->axis[0]>0 && engineInput->axis[1]>0 && (posy[0]>>SP_ACCURACY)>-6)
					choose_one = 4;
				else
				if (engineInput->axis[0]==0 && engineInput->axis[1]>0 && (posy[0]>>SP_ACCURACY)>-4)
					choose_one = 5;
				else
				if (engineInput->axis[0]<0 && engineInput->axis[1]>0 && (posy[0]>>SP_ACCURACY)>-6)
					choose_one = 6;
				else
				if (engineInput->axis[0]<0 && engineInput->axis[1]==0)
					choose_one = 7;
				else
				if (engineInput->axis[0]<0 && engineInput->axis[1]<0 && (posy[0]>>SP_ACCURACY)<6)
					choose_one = 8;
				else
					choose_one = 0;
			}
			else
			{
				if (choose_one>=0)
				{
					int x = (20-(posx[0]>>SP_ACCURACY)) & 15;
					int y = 3+(posy[0]>>SP_ACCURACY+1);
					play_switch();
					switch (choose_one)
					{
						case 1: initate_change(x,y,x,y+2); break;
						case 2: initate_change(x,y,(x+15) & 15,y+1); break;
						case 3: initate_change(x,y,(x+14) & 15,y); break;
						case 4: initate_change(x,y,(x+15) & 15,y-1); break;
						case 5: initate_change(x,y,x,y-2); break;
						case 6: initate_change(x,y,(x+1) & 15,y-1); break;
						case 7: initate_change(x,y,(x+2) & 15,y); break;
						case 8: initate_change(x,y,(x+1) & 15,y+1); break;
					}
					control_timeout = 200;
					choose_one = -1;
				}
			}
		}
	}
	
	calc_lettering(steps);
	calc_bordering(steps);
		
	w =(w+(steps*16))%(2*SP_PI);
	if (engineInput->button[SP_BUTTON_SELECT])
		return 1;
	return 0; 
}

int run_game(int playernumber_,int starAdd,void (*resize)(Uint16 w,Uint16 h))
{
	if (settings_get_control()==2)
		choose_one = -1;
	else
		choose_one = 0;
	insert_name = 0;
	control_timeout = 0;
	choosen_letter = 0;
	settings_reset_highscore_name(myhighscore_name);
	countdown = 3999;
	playernumber = playernumber_;
	timeStep = 1;
	star_add = starAdd;
	first_pointVis = NULL;

	switch (rand()%4)
	{
		case 0: change_music("Midnight Mediation","Nick May"); break;
		case 1: change_music("Cosmic Conundrum","Nick May"); break;
		case 2: change_music("Energetic Enigma","Nick May"); break;
		case 3: change_music("Impossible Paradox","Nick May"); break;
	}
	
	
	spLoop(draw_game,calc_game,10,resize,NULL);
	
	delete_pointVis();
	game_counter = 0;
	if (settings_get_stars_rotating()<2)
		return star_add;
	else
		return star_add + last_rotate;
}
