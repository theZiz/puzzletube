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
#include "game.h"
#include "lettering.h"
#include "settings.h"
#include <stdlib.h>
#include <string.h>

int playernumber;
GameMode mode;
int difficult;
int colornumber;
int countdown;
int game_counter;

int star_add;

/*pmesh stone_mesh;
pmesh stone_special_mesh;
pmesh stone_mesh_gp2x;
pmesh stone_special_mesh_gp2x;*/
//pmesh border_mesh;
//pmesh border_thin_mesh;
typedef SDL_Surface *PSDL_Surface;
PSDL_Surface stone_texture[9];
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

#define MAX_LETTERING 2000
SDL_Surface* momLettering;
int letteringTimeOut;

#define START_TIME 180000

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

char name[4]="AAA";
char insert_name;
int selected_letter;

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

void show_lettering(char* name)
{
  momLettering = get_lettering(name);
  letteringTimeOut = MAX_LETTERING;
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

Sint32 get_type_color_h(int type)
{
  if (type<6)
    return type*SP_PI/3;
  switch (type) //special stone
  {
    case 6: return w*10;
    case 7: return (spSin(w*13)+(1<<SP_ACCURACY)>>SP_HALF_ACCURACY)*(SP_PI/12>>SP_HALF_ACCURACY);
    case 8: return 0;
  }
}

Uint8 get_type_color_s(int type)
{
  if (type<6)
    return 255;
  switch (type) //special stone
  {
    case 6: return 64;
    case 7: return 255;
    case 8: return 0;
  }
}

Uint8 get_type_color_v(int type)
{
  if (type<6)
    return 255;
  switch (type) //special stone
  {
    case 6: return 255;
    case 7: return ((spSin(w*64)*63)>>SP_ACCURACY)+96;
    case 8: return 127;
  }
}

pwinsituation search_win_situation()
{
  pwinsituation finalresult = NULL;
  int x,y;
  for (x=0;x<16;x++)
    for (y=0;y<6;y++)
    {
      if (stone[y  ][ x      ].type==8 ||
          stone[y+1][ x      ].type==8 ||
          stone[y  ][(x+1)%16].type==8 ||
          stone[y+1][(x+1)%16].type==8)
        continue;
      if ((stone[y][x].type==6 && stone[y+1][x].type==6 && stone[y][(x+1)%16].type==6)
          ||
          (stone[y][x].type==6 && stone[y+1][x].type==6 &&
          (stone[y][(x+1)%16].type==stone[y+1][(x+1)%16].type || stone[y+1][(x+1)%16].type==6))
          ||
          (stone[y][x].type==6 &&
          (stone[y+1][x].type==stone[y  ][(x+1)%16].type || stone[y  ][(x+1)%16].type==6) &&
          (stone[y+1][x].type==stone[y+1][(x+1)%16].type || stone[y+1][(x+1)%16].type==6))
          ||
          (stone[y][x].type>=0 &&
          (stone[y][x].type==stone[y+1][ x      ].type || stone[y+1][ x      ].type==6) &&
          (stone[y][x].type==stone[y  ][(x+1)%16].type || stone[y  ][(x+1)%16].type==6) &&
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
          type = stone[y  ][(x+1)%16].type;
        if (type == 6)
          type = stone[y+1][(x+1)%16].type;
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
      stone[temp->y][temp->x].type=rand()%colornumber;
      stone[temp->y][temp->x].h=get_type_color_h(stone[temp->y][temp->x].type);
      stone[temp->y][temp->x].s=get_type_color_s(stone[temp->y][temp->x].type);
      stone[temp->y][temp->x].v=get_type_color_v(stone[temp->y][temp->x].type);
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


void prepare_game_objects(char complete,int colornumber_)
{
  points = 0;
  gameTime = START_TIME;
  realTime = 0;
  colornumber = colornumber_; 
  chain = 0;
  int a,y;
  for (y=0;y<7;y++)
    for (a=0;a<16;a++)
    {
      stone[y][a].type=rand()%colornumber;
      stone[y][a].h=get_type_color_h(stone[y][a].type);
      stone[y][a].s=get_type_color_s(stone[y][a].type);
      stone[y][a].v=get_type_color_v(stone[y][a].type);
      stone[y][a].falling=0;
      stone[y][a].new=0;
    }
  posx[0]=0;
  posy[0]=-2<<SP_ACCURACY;
  posx[3]=posx[0];
  posy[3]=posy[0];
  posx[1]=0;
  posy[1]=0;
  posx[2]=0;
  posy[2]=2<<SP_ACCURACY;
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
  if (complete)
  {
    stone_texture[0] = spLoadSurface("./images/stone1.png");
    stone_texture[1] = spLoadSurface("./images/stone2.png");
    stone_texture[2] = spLoadSurface("./images/stone3.png");
    stone_texture[3] = spLoadSurface("./images/stone4.png");
    stone_texture[4] = spLoadSurface("./images/stone5.png");
    stone_texture[5] = spLoadSurface("./images/stone6.png");
    stone_texture[6] = spLoadSurface("./images/stone7.png");
    stone_texture[7] = spLoadSurface("./images/stone8.png");
    stone_texture[8] = spLoadSurface("./images/stone9.png");
    /*stone_mesh_gp2x=loadMesh("./data/stone_gp2x.obj");
    stone_special_mesh_gp2x=loadMesh("./data/stone_special_gp2x.obj");
    stone_mesh=loadMesh("./data/stone.obj");
    stone_special_mesh=loadMesh("./data/stone_special.obj");*/
    //border_mesh=loadMesh("./data/border.obj");  
    //border_thin_mesh=loadMesh("./data/border_thin.obj");  
    resize_particle(spGetWindowSurface()->w,spGetWindowSurface()->h);
    init_stars();
  }
  init_light();
}

void delete_game_objects()
{
  int i;
  for (i = 0;i<9;i++)
    SDL_FreeSurface(stone_texture[i]);
  //freeMesh(stone_mesh);
  //freeMesh(stone_special_mesh);
  //freeMesh(border_mesh);
  //freeMesh(border_thin_mesh);  
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
  int new_points = 0;
  pwinsituation situation;
  char found = 0;
  char type_found[12];
  while ((situation=search_win_situation())!=NULL)
  {
    memset(type_found,0,12);
    type_found[6]=1;
    char count = 0;
    found = 1;
    pwinsituation temp=situation;
    int i = 0;
    while (temp!=NULL)
    {
      if (stone[temp->y][temp->x].type>=0 && type_found[stone[temp->y][temp->x].type] == 0)
      {
        printf("__%i__\n",stone[temp->y][temp->x].type);
        count++;
        type_found[stone[temp->y][temp->x].type] = 1;
      }

      stone[temp->y][temp->x].type=-1;
      play_explosion();
      if (settings_get_particles())
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
      i++;
      printf("%i:%i\n",temp->x,temp->y);
      temp=temp->next; 
    }
      if (i<7)
        {}
      else
      if (i<9)
        show_lettering("well");
      else
      if (i<12)
        show_lettering("great");
      else
      if (i<16)
        show_lettering("fantastic");
      else
      if (i<20)
        show_lettering("incredible");
      else
        show_lettering("supercalifragilisticexpialidocious");
      if (i>0)
        new_points = (int)(pow((float)(i),1.5)*100.0);
      delete_win_situation(situation);
      if (count>1)
      {
        show_lettering("combo");
        new_points*=3;
      }
  }
  if (found)
  {
    chain++;
    chain_break=100;
    if (chain>1)
    {
      switch (chain)
      {
        case 2: show_lettering("chain");break;
        case 3: show_lettering("chain_2x"); break;
        case 4: show_lettering("chain_3x"); break;
        case 5: show_lettering("chain_4x"); break;
        case 6: show_lettering("chain_5x"); break;
        default: show_lettering("hyper_chain"); break;
      }
      new_points = new_points * 3 / 2;
    }
    if (new_points>0)
      add_pointVis(new_points);
    if (mode & timeMode)
    {
      gameTime += new_points;
      if (gameTime > START_TIME)
        gameTime = START_TIME;
    }
    else
      points += new_points;
  }
}

#define PARTICLE_SPEED 1500

void draw_particle(int posx,int posy,Sint32 r,int time,SDL_Surface* particle)
{
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
}

#define PARTICLE_SPEED2 2500

void draw_particle2(int posx,int posy,Sint32 r,int time,SDL_Surface* particle)
{
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
}

int last_rotate = 0;

void draw_game(void)
{
  spFontPointer font = settings_get_font();
  spFontPointer small_font = settings_get_small_font();
  spFontPointer middle_font = settings_get_middle_font();
  Sint32* modellViewMatrix=spGetMatrix();
  //plight light=engineGetLightPointer();
  int engineWindowX=spGetWindowSurface()->w;
  int engineWindowY=spGetWindowSurface()->h;

  spClearTarget(0);
  spResetZBuffer();
  spIdentity();
  spSetZSet(0);
  spSetZTest(0);
    
  Sint32 matrix[16];
  
  spTranslate(0,0,-20<<SP_ACCURACY);

  if (settings_get_stars_rotating()==1)
  {
    spRotate(0,1<<SP_ACCURACY,0,star_add);
    draw_stars();
    spRotate(0,-1<<SP_ACCURACY,0,star_add);
  }

  spSetZSet(1);
  spSetZTest(1);

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

  if (settings_get_stars_rotating()==2)
  {
    spRotate(0,1<<SP_ACCURACY,0,star_add);
    draw_stars();
    spRotate(0,-1<<SP_ACCURACY,0,star_add);
  }
  //spTranslate(0,spSin(w<<3)>>2,0);

  spRotate(1<<SP_ACCURACY,0,0,(spSin(w<<3)>>SP_HALF_ACCURACY)*(SP_PI>>SP_HALF_ACCURACY)>>5);
  spRotate(0,1<<SP_ACCURACY,0,(spCos(w<<3)>>SP_HALF_ACCURACY)*(SP_PI>>SP_HALF_ACCURACY)>>5);
  
  int meta_a,a,y;
  //if ((posx[0]>>SP_ACCURACY) == (24-a)%16)
  for (meta_a=16-(posx[0]>>SP_ACCURACY);meta_a<16-(posx[0]>>SP_ACCURACY)+16;meta_a++)
  {
    a = meta_a & 15;
    for (y=-6;y<=6;y+=2)
    {
      if (stone[(y>>1)+3][a].type<0)
        continue;
      stone[(y>>1)+3][a].h = get_type_color_h(stone[(y>>1)+3][a].type);
      stone[(y>>1)+3][a].s = get_type_color_s(stone[(y>>1)+3][a].type);
      stone[(y>>1)+3][a].v = get_type_color_v(stone[(y>>1)+3][a].type);
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
          case  14: 
          case - 2: new_a+=sign*2*((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
          case  15:
          case - 1: new_a+=sign*  ((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
          case -15: 
          case   1: new_a-=sign*  ((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
          case -14:
          case   2: new_a-=sign*2*((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
        }
        switch (change->ay-change->by)
        {
          case -2: new_y+=sign*4*((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
          case -1: new_y+=sign*2*((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
          case  1: new_y-=sign*2*((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
          case  2: new_y-=sign*4*((CHANGE_TIME-change->progress)<<SP_ACCURACY)/CHANGE_TIME; break;
        }
        px=(spCos((new_a>>SP_HALF_ACCURACY+1)*(SP_PI>>SP_HALF_ACCURACY+2))>>SP_HALF_ACCURACY)*((1<<SP_ACCURACY)-sign*(spSin(change->progress*SP_PI/CHANGE_TIME)/5)>>SP_HALF_ACCURACY)*5;
        py=new_y;
        pz=(spSin((new_a>>SP_HALF_ACCURACY+1)*(SP_PI>>SP_HALF_ACCURACY+2))>>SP_HALF_ACCURACY)*((1<<SP_ACCURACY)-sign*(spSin(change->progress*SP_PI/CHANGE_TIME)/5)>>SP_HALF_ACCURACY)*5;
      }
      if (stone[(y>>1)+3][a].falling)
      {
        py-=(2*(FALL_TIME-stone[(y>>1)+3][a].falling)<<SP_ACCURACY)/FALL_TIME;
      }
      
      spTranslate(px,py,pz);
      spRotate(0,1<<SP_ACCURACY,0,2*SP_PI+SP_PI/2-(a*SP_PI>>3));
      int s=stone[(y>>1)+3][a].s;

      if (stone[(y>>1)+3][a].type==stone[(y>>1)+3][(a+8)%16].type)
        spRotate(0,0,1<<SP_ACCURACY,spSin(w*64)/2);
        //spRotate(0,0,1<<SP_ACCURACY,SP_PI/4);

      int v=stone[(y>>1)+3][a].v-64+(2*spSin((-posx[0]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)-((a+8)*SP_PI>>3))>>(SP_ACCURACY-5));
      
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
      if (settings_get_stone_quality() == 2)
      {
        spSetCulling(0);
        spBindTexture(stone_texture[stone[(y>>1)+3][a].type]);
        spQuadTex3D(-7<<SP_ACCURACY-3,-7<<SP_ACCURACY-3,0,0,stone_texture[0]->h-SP_FONT_EXTRASPACE-1,
                     7<<SP_ACCURACY-3,-7<<SP_ACCURACY-3,0,stone_texture[0]->w-SP_FONT_EXTRASPACE-1,stone_texture[0]->h-SP_FONT_EXTRASPACE-1,
                     7<<SP_ACCURACY-3, 7<<SP_ACCURACY-3,0,stone_texture[0]->w-SP_FONT_EXTRASPACE-1,0,
                    -7<<SP_ACCURACY-3, 7<<SP_ACCURACY-3,0,0,0,spGetHSV(0,0,255-64+(2*spSin((-posx[0]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)-((a+8)*SP_PI>>3))>>(SP_ACCURACY-5))));
        spSetCulling(1);
      }
      else
      if (settings_get_stone_quality() == 1)
      {
        spSetCulling(0);
        spQuad3D(-5<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,
                  5<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,
                  5<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,
                 -5<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,spGetHSV(stone[(y>>1)+3][a].h,s,v));
        spQuad3D(-7<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,
                 -5<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,
                 -5<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,
                 -7<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,0);
        spQuad3D( 7<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,
                  5<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,
                  5<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,
                  7<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,0);
        spQuad3D(-5<<SP_ACCURACY-3,-7<<SP_ACCURACY-3,0,
                  5<<SP_ACCURACY-3,-7<<SP_ACCURACY-3,0,
                  5<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,
                 -5<<SP_ACCURACY-3,-5<<SP_ACCURACY-3,0,0);
        spQuad3D(-5<<SP_ACCURACY-3, 7<<SP_ACCURACY-3,0,
                  5<<SP_ACCURACY-3, 7<<SP_ACCURACY-3,0,
                  5<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,
                 -5<<SP_ACCURACY-3, 5<<SP_ACCURACY-3,0,0);
        spSetCulling(1);
      }
      else
      {
        spSetCulling(0);
        spQuad3D(-3<<SP_ACCURACY-2,-3<<SP_ACCURACY-2,0,
                    3<<SP_ACCURACY-2,-3<<SP_ACCURACY-2,0,
                    3<<SP_ACCURACY-2, 3<<SP_ACCURACY-2,0,
                   -3<<SP_ACCURACY-2, 3<<SP_ACCURACY-2,0,spGetHSV(stone[(y>>1)+3][a].h,s,v));
        spSetCulling(1);
      }
      memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);
    }
  }
  //particles
  pparticle particle=firstparticle;
  while (particle)
  {
    memcpy(matrix,modellViewMatrix,sizeof(Sint32)*16);
    spTranslate(particle->x,particle->y,particle->z);
    spRotate(1<<SP_ACCURACY,0,0,particle->rotx);
    spRotate(0,1<<SP_ACCURACY,0,particle->roty);
    spRotate(0,0,1<<SP_ACCURACY,particle->rotz);
    spQuad3D(-(particle->age<<SP_ACCURACY)/PARTICLE_AGE/4,
             -(particle->age<<SP_ACCURACY)/PARTICLE_AGE/4,0,
              (particle->age<<SP_ACCURACY)/PARTICLE_AGE/4,
             -(particle->age<<SP_ACCURACY)/PARTICLE_AGE/4,0,
              (particle->age<<SP_ACCURACY)/PARTICLE_AGE/4,
              (particle->age<<SP_ACCURACY)/PARTICLE_AGE/4,0,
             -(particle->age<<SP_ACCURACY)/PARTICLE_AGE/4,
              (particle->age<<SP_ACCURACY)/PARTICLE_AGE/4,0,
              spGetHSV(particle->h,particle->s,particle->v));
    memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);
    particle=particle->next;
  }
    
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

  spBlit3D(spCos(-(pointposx[(  TIMEOUT-1+pointstart+2*TIMEOUT-TIMEOUT/4)%(2*TIMEOUT)]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,pointposy[(  TIMEOUT-1+pointstart+2*TIMEOUT-TIMEOUT/4)%(2*TIMEOUT)],spSin((pointposx[(  TIMEOUT-1+pointstart+2*TIMEOUT-TIMEOUT/4)%(2*TIMEOUT)]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,getMiddleParticle());
  spBlit3D(spCos(-(pointposx[(  TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,pointposy[(  TIMEOUT-1+pointstart)%(2*TIMEOUT)],spSin((pointposx[(  TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,getMiddleParticle());
  spBlit3D(spCos(-(pointposx[(  TIMEOUT-1+pointstart+TIMEOUT/4)%(2*TIMEOUT)]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,pointposy[(  TIMEOUT-1+pointstart+TIMEOUT/4)%(2*TIMEOUT)],spSin((pointposx[(  TIMEOUT-1+pointstart+TIMEOUT/4)%(2*TIMEOUT)]>>SP_HALF_ACCURACY+2)*(SP_PI>>SP_HALF_ACCURACY+1)+SP_PI/2)*22>>2,getMiddleParticle());

  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(game_counter+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

  
  char buffer[256];
  
  spSetZSet(0);
  spSetZTest(0);
  
  //HUD on the left side
  SDL_Surface* shown = NULL;

  //setting "shown"
  if (countdown>0)
    switch ((countdown)/1000)
    {
      case  3: shown = get_lettering("3"); break;
      case  2: shown = get_lettering("2"); break;
      case  1: shown = get_lettering("1"); break;
      case  0: shown = get_lettering("go"); break;
    }
  else
  {
    if ((mode & timeMode) && gameTime<START_TIME/5 && momLettering == NULL)
      shown = get_lettering("hurry_up");
    else
      shown = momLettering;
  }

  //Name
  if (insert_name)
  {
    //drawtextMX(engineGetSurface(SURFACE_SURFACE),engineWindowX/7, 9*engineWindowY/20,"Name:",engineGetSurface(SURFACE_KEYMAP));
    //drawtextMX(engineGetSurface(SURFACE_SURFACE),engineWindowX/7,11*engineWindowY/20,name,engineGetSurface(SURFACE_KEYMAP));    
    shown = NULL;
  }

  
  if (shown != NULL)
      spBlitSurface(1*engineWindowX/7,engineWindowY/2,-1,shown);
  
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
  spFontDrawMiddle(6*engineWindowX/7,1*engineWindowY/16,-1,"Game Mode:",middle_font);
  if (mode & timeMode)
    sprintf(buffer,"Time Stole");
  else
    sprintf(buffer,"Points");
  spFontDrawMiddle(6*engineWindowX/7,2*engineWindowY/16,-1,buffer,middle_font);
  if (mode & timeMode)
  {
    spFontDrawMiddle(6*engineWindowX/7,4*engineWindowY/16,-1,"Time Past:",middle_font);
    sprintf(buffer,"%i Seconds",realTime/1000);
    spFontDrawMiddle(6*engineWindowX/7,5*engineWindowY/16,-1,buffer,middle_font);
  }
  else
  {
    spFontDrawMiddle(6*engineWindowX/7,4*engineWindowY/16,-1,"Points:",middle_font);
    sprintf(buffer,"%i",points);
    spFontDrawMiddle(6*engineWindowX/7,5*engineWindowY/16,-1,buffer,middle_font);
  }

  spBlitSurfacePart(6*engineWindowX/7,13*engineWindowY/32,-1,getTimeSurface(),0,0,gameTime*getTimeSurface()->w/START_TIME,getTimeSurface()->h);

  spFontDrawMiddle(6*engineWindowX/7,20*engineWindowY/32,-1,"Select:",small_font);
  spFontDrawMiddle(6*engineWindowX/7,22*engineWindowY/32,-1,"Pause",small_font);
  spFontDrawMiddle(6*engineWindowX/7,25*engineWindowY/32,-1,"Slart:",small_font);
  spFontDrawMiddle(6*engineWindowX/7,27*engineWindowY/32,-1,"Back to Menu",small_font);


  sprintf(buffer,"fps: %i",spGetFPS());
  spFontDrawRight(engineWindowX,engineWindowY-small_font->maxheight,-1,buffer,small_font);

  draw_music();
  spFlip();
}

int calc_game(Uint32 steps)
{
  PspInput engineInput = spGetInput();
  if (engineInput->button[SP_BUTTON_SELECT])
  {
    pause=!pause;
    engineInput->button[SP_BUTTON_SELECT]=0;
  }
  
  if (insert_name)
  {
    if (engineInput->axis[0]>0)
    {
      selected_letter = (selected_letter+1)%3;
      engineInput->axis[0] = 0;
    }
    if (engineInput->axis[0]<0)
    {
      selected_letter = (selected_letter+2)%3;
      engineInput->axis[0] = 0;
    }
    if (engineInput->axis[1]>0)
    {
      name[selected_letter] = (name[selected_letter]-'A' +25)%26+'A';
      engineInput->axis[1] = 0;
    }
    if (engineInput->axis[1]<0)
    {
      name[selected_letter] = (name[selected_letter]-'A' +1)%26+'A';
      engineInput->axis[1] = 0;
    }
    
    if (engineInput->button[SP_BUTTON_START])
    {
      if (mode & timeMode)
        insert_highscore(mode,colornumber-4,difficult,name,realTime);
      else
        insert_highscore(mode,colornumber-4,difficult,name,points);
      highscore_save();
      return 1;
    }
    return 0;
  }
  
  if (pause)
    return 0;
  calc_music(steps);
  if (countdown>0)
  {
    countdown-=steps;
    return 0;
  }
  if (letteringTimeOut>0)
    letteringTimeOut -= steps;
  else
    momLettering = NULL;
  calc_pointVis(steps);
  game_counter+=steps;
  int i;
  for (i=0;i<steps;i++)
  {
    realTime++;
    if (mode & timeMode)
    {
      if (realTime%((11-difficult)*6000) == 0)
        timeStep = 2*timeStep-timeStep/2; 
    }
    gameTime -= timeStep;
    if (gameTime<=0)
    {
      move_sound_off();
      rotating_sound_off();
      /*if (((mode & timeMode) && realTime > get_highscore(mode,colornumber-4,difficult,5)) ||
          (((mode & timeMode)==0) && points > get_highscore(mode,colornumber-4,difficult,5)))
        insert_name = 1;      
      else*/
        return 1;
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
            stone[y][a].type=rand()%colornumber;
            stone[y][a].h=get_type_color_h(stone[y][a].type);
            stone[y][a].s=get_type_color_s(stone[y][a].type);
            stone[y][a].v=get_type_color_v(stone[y][a].type);
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
          stone[y-1][a]=stone[y  ][a];
          stone[y-1][a].falling=0;
          if (y==6)
          {
            stone[y][a].type=rand()%colornumber;
            stone[y][a].h=get_type_color_h(stone[y][a].type);
            stone[y][a].s=get_type_color_s(stone[y][a].type);
            stone[y][a].v=get_type_color_v(stone[y][a].type);
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
      if (engineInput->axis[1]<0 && (((posy[0]>>SP_ACCURACY)>-6 && direction!=3) || (direction==3 && (posy[2]>>SP_ACCURACY)>-6)))
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
      if (engineInput->axis[1]>0 && (((posy[0]>>SP_ACCURACY)< 6 && direction!=2) || (direction==2 && (posy[2]>>SP_ACCURACY)< 6)))
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
  }
    
  w+=(steps*16)%(2*SP_PI);
  if (engineInput->button[SP_BUTTON_START])
    return 1;
  return 0; 
}

int run_game(int playernumber_,GameMode mode_,int difficult_ /*0..9*/,int starAdd,void (*resize)(Uint16 w,Uint16 h))
{
  countdown = 4000;
  playernumber = playernumber_;
  mode = mode_;
  difficult = difficult_;
  timeStep = difficult/2+2;
  star_add = starAdd;
  first_pointVis = NULL;
  insert_name = 0;
  selected_letter = 0;
  
  switch (rand()%4)
  {
    case 0: change_music("Midnight Mediation","Nick May"); break;
    case 1: change_music("Cosmic Conundrum","Nick May"); break;
    case 2: change_music("Energetic Enigma","Nick May"); break;
    case 3: change_music("Impossible Paradox","Nick May"); break;
  }
  
  spLoop(draw_game,calc_game,10,resize);
  
  delete_pointVis();
  game_counter = 0;
  if (settings_get_stars_rotating()<2)
    return star_add;
  else
    return star_add + last_rotate;
}
