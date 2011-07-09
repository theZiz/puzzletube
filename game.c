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

int playernumber;
GameMode mode;
int difficult;
int colornumber;

pmesh stone_mesh;
pmesh stone_special_mesh;
pmesh stone_mesh_gp2x;
pmesh stone_special_mesh_gp2x;
//pmesh border_mesh;
//pmesh border_thin_mesh;
SDL_Surface *dummySurface = NULL;
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

#define START_TIME 180000

pchange firstchange;
pparticle firstparticle;

char pause=0;

int chain;
int chain_break;
int oldchain;

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
    return type*MY_PI/3;
  switch (type) //special stone
  {
    case 6: return w*10;
    case 7: return (mysin(w*13)+(1<<ACCURACY)>>HALF_ACCURACY)*(MY_PI/12>>HALF_ACCURACY);
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
    return 200;
  switch (type) //special stone
  {
    case 6: return 255;
    case 7: return ((mysin(w*64)*63)>>ACCURACY)+96;
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
  plight light=engineGetLightPointer();  
  light[0].r=255;
  light[0].g=255;
  light[0].b=255;
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
  posy[0]=-2<<ACCURACY;
  posx[3]=posx[0];
  posy[3]=posy[0];
  posx[1]=0;
  posy[1]=0;
  posx[2]=0;
  posy[2]=2<<ACCURACY;
  int i;
  for (i=0;i<2*TIMEOUT;i++)
  {
    pointposx[i]=0; 
    pointposy[i]=(i-TIMEOUT<<ACCURACY)/(TIMEOUT/2); 
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
    stone_mesh_gp2x=loadMesh("./data/stone_gp2x.obj");
    stone_special_mesh_gp2x=loadMesh("./data/stone_special_gp2x.obj");
    stone_mesh=loadMesh("./data/stone.obj");
    stone_special_mesh=loadMesh("./data/stone_special.obj");
    //border_mesh=loadMesh("./data/border.obj");  
    //border_thin_mesh=loadMesh("./data/border_thin.obj");  
    resize_particle(engineGetWindowX(),engineGetWindowY());
    dummySurface = IMG_Load("./images/dummy.png");
  }
  init_stars();
  init_light();
}

void delete_game_objects()
{
  freeMesh(stone_mesh);
  freeMesh(stone_special_mesh);
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
  particle->dx=rand()%(1<<ACCURACY-7)-(1<<ACCURACY-8);
  particle->dy=rand()%(1<<ACCURACY-7)-(1<<ACCURACY-8);
  particle->dz=rand()%(1<<ACCURACY-7)-(1<<ACCURACY-8);
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
    particle->dy-=1<<ACCURACY-15;
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
        new_particle(mycos((temp->x*MY_PI>>3)-MY_PI/32)*5,((temp->y-3)*2<<ACCURACY)-(1<<ACCURACY-1),mysin((temp->x*MY_PI>>3)-MY_PI/32)*5,
                     stone[temp->y][temp->x].h,stone[temp->y][temp->x].s,stone[temp->y][temp->x].v,
                     0,2*MY_PI+MY_PI/2-(temp->x*MY_PI>>3),0);
        new_particle(mycos((temp->x*MY_PI>>3)+MY_PI/32)*5,((temp->y-3)*2<<ACCURACY)-(1<<ACCURACY-1),mysin((temp->x*MY_PI>>3)+MY_PI/32)*5,
                     stone[temp->y][temp->x].h,stone[temp->y][temp->x].s,stone[temp->y][temp->x].v,
                     0,2*MY_PI+MY_PI/2-(temp->x*MY_PI>>3),0);
        new_particle(mycos((temp->x*MY_PI>>3)-MY_PI/32)*5,((temp->y-3)*2<<ACCURACY)+(1<<ACCURACY-1),mysin((temp->x*MY_PI>>3)-MY_PI/32)*5,
                     stone[temp->y][temp->x].h,stone[temp->y][temp->x].s,stone[temp->y][temp->x].v,
                     0,2*MY_PI+MY_PI/2-(temp->x*MY_PI>>3),0);
        new_particle(mycos((temp->x*MY_PI>>3)+MY_PI/32)*5,((temp->y-3)*2<<ACCURACY)+(1<<ACCURACY-1),mysin((temp->x*MY_PI>>3)+MY_PI/32)*5,
                     stone[temp->y][temp->x].h,stone[temp->y][temp->x].s,stone[temp->y][temp->x].v,
                     0,2*MY_PI+MY_PI/2-(temp->x*MY_PI>>3),0);
      }
      i++;
      printf("%i:%i\n",temp->x,temp->y);
      temp=temp->next; 
    }
    printf("You broke %i Stones - ",i);
      if (i<7)
        printf("\n");
      else
      if (i<9)
        printf("well\n");
      else
      if (i<12)
        printf("great\n");
      else
      if (i<16)
        printf("fantastic\n");
      else
      if (i<20)
        printf("incredible\n");
      else
        printf("supercalifragilisticexpialidocious\n");
    delete_win_situation(situation);
    if (count>0)
      printf("count: %i (combo if >1)\n",count);
  }
  if (found)
  {
    chain++;
    chain_break=100;
    printf("Chain %i\n",chain);
  }
}

#define PARTICLE_SPEED 1500

void draw_particle(int posx,int posy,Sint32 r,int time,SDL_Surface* particle)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  Sint32 matrix[16];
  memcpy(matrix,modellViewMatrix,sizeof(Sint32)*16);
    engineTranslate(mycos(-(posx>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,posy,mysin((posx>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2);
    engineRotate(0,1<<ACCURACY,0,(posx>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1));
    if (time<PARTICLE_SPEED/4)
      engineDrawSurface(r*time/(PARTICLE_SPEED/8)-r,r,0,particle);
    else
    if (time<PARTICLE_SPEED/2)
      engineDrawSurface(r,r-r*(time-PARTICLE_SPEED/4)/(PARTICLE_SPEED/8),0,particle);
    else
    if (time<3*PARTICLE_SPEED/4)
      engineDrawSurface(r-r*(time-PARTICLE_SPEED/2)/(PARTICLE_SPEED/8),-r,0,particle);
    else
      engineDrawSurface(-r,r-r*(PARTICLE_SPEED-time)/(PARTICLE_SPEED/8),0,particle);
    
  memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);
}

#define PARTICLE_SPEED2 2500

void draw_particle2(int posx,int posy,Sint32 r,int time,SDL_Surface* particle)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  Sint32 matrix[16];
  memcpy(matrix,modellViewMatrix,sizeof(Sint32)*16);
    engineTranslate(mycos(-(posx>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,posy,mysin((posx>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2);
    engineRotate(0,1<<ACCURACY,0,(posx>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1));
    if (time<PARTICLE_SPEED2/4)
      engineDrawSurface(r-r*time/(PARTICLE_SPEED2/8),r,0,particle);
    else
    if (time<PARTICLE_SPEED2/2)
      engineDrawSurface(r,r*(time-PARTICLE_SPEED2/4)/(PARTICLE_SPEED2/8)-r,0,particle);
    else
    if (time<3*PARTICLE_SPEED2/4)
      engineDrawSurface(r*(time-PARTICLE_SPEED2/2)/(PARTICLE_SPEED2/8)-r,-r,0,particle);
    else
      engineDrawSurface(-r,r*(PARTICLE_SPEED2-time)/(PARTICLE_SPEED2/8)-r,0,particle);
    
  memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);
}

void draw_game(void)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  plight light=engineGetLightPointer();
  int engineWindowX=engineGetWindowX();
  int engineWindowY=engineGetWindowY();

  //engineClearScreen(getHSV(w,32,255));
  engineClearScreen(0);
  setModellViewMatrixIdentity();
    
  Sint32 matrix[16];
  

  engineTranslate(0,0,-20<<ACCURACY);

  if (settings_get_stars_rotating()==1)
    draw_stars();

  engineRotate(0,1<<ACCURACY,0,(-posx[0]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1));

 
  int add=1;
  if (is_change>0)
  {
    if (is_change<2)
      add++;
    engineRotate(0,-1<<ACCURACY,0,((timeout<<HALF_ACCURACY-1)/TIMEOUT)*(MY_PI>>HALF_ACCURACY+add));
  }
  if (is_change<0)
  {
    if (is_change>-2)
      add++;
    engineRotate(0, 1<<ACCURACY,0,((timeout<<HALF_ACCURACY-1)/TIMEOUT)*(MY_PI>>HALF_ACCURACY+add));
  }

  int a,y;

  if (settings_get_stars_rotating()==2)
    draw_stars();
  //engineTranslate(0,mysin(w<<3)>>2,0);

  engineRotate(1<<ACCURACY,0,0,(mysin(w<<3)>>HALF_ACCURACY)*(MY_PI>>HALF_ACCURACY)>>5);
  engineRotate(0,1<<ACCURACY,0,(mycos(w<<3)>>HALF_ACCURACY)*(MY_PI>>HALF_ACCURACY)>>5);
  
  for (a=0;a<16;a++)
    for (y=-6;y<=6;y+=2)
    {
      if (stone[(y>>1)+3][a].type<0)
        continue;
      stone[(y>>1)+3][a].h = get_type_color_h(stone[(y>>1)+3][a].type);
      stone[(y>>1)+3][a].s = get_type_color_s(stone[(y>>1)+3][a].type);
      stone[(y>>1)+3][a].v = get_type_color_v(stone[(y>>1)+3][a].type);
      memcpy(matrix,modellViewMatrix,sizeof(Sint32)*16);
      pchange change=is_in_change(a,3+y/2);
      Sint32 px=mycos(a*MY_PI>>3)*5;
      Sint32 py=y<<ACCURACY;
      Sint32 pz=mysin(a*MY_PI>>3)*5;
      if (change)
      {
        int sign=1;
        Sint32 new_a=a<<ACCURACY;
        Sint32 new_y=y<<ACCURACY;
        if (change->bx==a && change->by==3+y/2)
          sign=-1;
        switch (change->ax-change->bx)
        {
          case  14: 
          case - 2: new_a+=sign*2*((CHANGE_TIME-change->progress)<<ACCURACY)/CHANGE_TIME; break;
          case  15:
          case - 1: new_a+=sign*  ((CHANGE_TIME-change->progress)<<ACCURACY)/CHANGE_TIME; break;
          case -15: 
          case   1: new_a-=sign*  ((CHANGE_TIME-change->progress)<<ACCURACY)/CHANGE_TIME; break;
          case -14:
          case   2: new_a-=sign*2*((CHANGE_TIME-change->progress)<<ACCURACY)/CHANGE_TIME; break;
        }
        switch (change->ay-change->by)
        {
          case -2: new_y+=sign*4*((CHANGE_TIME-change->progress)<<ACCURACY)/CHANGE_TIME; break;
          case -1: new_y+=sign*2*((CHANGE_TIME-change->progress)<<ACCURACY)/CHANGE_TIME; break;
          case  1: new_y-=sign*2*((CHANGE_TIME-change->progress)<<ACCURACY)/CHANGE_TIME; break;
          case  2: new_y-=sign*4*((CHANGE_TIME-change->progress)<<ACCURACY)/CHANGE_TIME; break;
        }
        px=(mycos((new_a>>HALF_ACCURACY+1)*(MY_PI>>HALF_ACCURACY+2))>>HALF_ACCURACY)*((1<<ACCURACY)-sign*(mysin(change->progress*MY_PI/CHANGE_TIME)/5)>>HALF_ACCURACY)*5;
        py=new_y;
        pz=(mysin((new_a>>HALF_ACCURACY+1)*(MY_PI>>HALF_ACCURACY+2))>>HALF_ACCURACY)*((1<<ACCURACY)-sign*(mysin(change->progress*MY_PI/CHANGE_TIME)/5)>>HALF_ACCURACY)*5;
      }
      if (stone[(y>>1)+3][a].falling)
      {
        py-=(2*(FALL_TIME-stone[(y>>1)+3][a].falling)<<ACCURACY)/FALL_TIME;
      }
      
      engineTranslate(px,py,pz);
      engineRotate(0,1<<ACCURACY,0,2*MY_PI+MY_PI/2-(a*MY_PI>>3));
      int s=stone[(y>>1)+3][a].s;

      //if (stone[(y>>1)+3][a].type==stone[(y>>1)+3][(a+8)%16].type)
      //  engineRotate(1<<ACCURACY,0,0,mysin(w*32)/2);

      int v=stone[(y>>1)+3][a].v-64+(2*mysin((-posx[0]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)-((a+8)*MY_PI>>3))>>(ACCURACY-5));

      if (stone[(y>>1)+3][a].new)
      {
        engineScale((NEW_TIME-stone[(y>>1)+3][a].new<<ACCURACY)/NEW_TIME,(NEW_TIME-stone[(y>>1)+3][a].new<<ACCURACY)/NEW_TIME,(NEW_TIME-stone[(y>>1)+3][a].new<<ACCURACY)/NEW_TIME);
        
      }

      /*if ((y+1>>1)==((posy[0]>>ACCURACY)+1>>1) && a==(20-((posx[0]+(1<<ACCURACY-1))>>ACCURACY))%16)
      {
        if (timeout>TIMEOUT/2)
        {
          engineRotate(0,0,1<<ACCURACY,mysin(w*32)/8);
          engineScale(((TIMEOUT/2+timeout)<<ACCURACY)/TIMEOUT,
                      ((TIMEOUT/2+timeout)<<ACCURACY)/TIMEOUT,
                      ((TIMEOUT/2+timeout)<<ACCURACY)/TIMEOUT);
          v=v/2;
        }
        else
        {
          engineScale(((TIMEOUT+TIMEOUT/2-timeout)<<ACCURACY)/TIMEOUT,
                      ((TIMEOUT+TIMEOUT/2-timeout)<<ACCURACY)/TIMEOUT,
                      ((TIMEOUT+TIMEOUT/2-timeout)<<ACCURACY)/TIMEOUT);
          engineRotate(0,0,1<<ACCURACY,(TIMEOUT/2-timeout)*mysin(w*32)/8/(TIMEOUT/2));
          v=((TIMEOUT/2)+timeout)*v/TIMEOUT;
        }
        //s=s*6/8;
      }
      if ((y+1>>1)==((posy[1]>>ACCURACY)+1>>1) && a==(20-((posx[1]+(1<<ACCURACY-1))>>ACCURACY))%16)
      {
        v=v/2;
        engineRotate(0,0,1<<ACCURACY,mysin(w*32)/8);
        //s=s*6/8;
      }
      if ((y+1>>1)==((posy[2]>>ACCURACY)+1>>1) && a==(20-((posx[2]+(1<<ACCURACY-1))>>ACCURACY))%16)
      {
        if (timeout>TIMEOUT/2)
        {
          engineRotate(0,0,1<<ACCURACY,(timeout-TIMEOUT/2)*mysin(w*32)/8/(TIMEOUT/2));
          engineScale(((TIMEOUT/2+timeout)<<ACCURACY)/TIMEOUT,
                      ((TIMEOUT/2+timeout)<<ACCURACY)/TIMEOUT,
                      ((TIMEOUT/2+timeout)<<ACCURACY)/TIMEOUT);
          v=(TIMEOUT+(TIMEOUT/2)-timeout)*v/TIMEOUT;
        }
        else
        {
          engineRotate(0,0,1<<ACCURACY,mysin(w*32)/8);
          engineScale(((TIMEOUT+TIMEOUT/2-timeout)<<ACCURACY)/TIMEOUT,
                      ((TIMEOUT+TIMEOUT/2-timeout)<<ACCURACY)/TIMEOUT,
                      ((TIMEOUT+TIMEOUT/2-timeout)<<ACCURACY)/TIMEOUT);
          v=v/2;            
        }        
        //s=s*6/8;
      }*/
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
        if (stone[(y>>1)+3][a].type==stone[(y>>1)+3][(a+8)%16].type)
          drawMesh(stone_special_mesh,getHSV(stone[(y>>1)+3][a].h,s,v));
        else
          drawMesh(stone_mesh,getHSV(stone[(y>>1)+3][a].h,s,v));
      }
      else
      if (settings_get_stone_quality() == 1)
      {
        if (stone[(y>>1)+3][a].type==stone[(y>>1)+3][(a+8)%16].type)
          drawMesh(stone_special_mesh_gp2x,getHSV(stone[(y>>1)+3][a].h,s,v));
        else
          drawMesh(stone_mesh_gp2x,getHSV(stone[(y>>1)+3][a].h,s,v));
      }
      else
      {
        Sint32 frontback =
          mysin(-(-posx[0]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+(a*MY_PI>>3));
        if (stone[(y>>1)+3][a].type==stone[(y>>1)+3][(a+8)%16].type)
        { 
          if (frontback > 0) //Front
          engineTriangle( 3<<ACCURACY-2, 3<<ACCURACY-2,0,
                         -3<<ACCURACY-2, 3<<ACCURACY-2,0,
                          0<<ACCURACY-2,-3<<ACCURACY-2,0,getHSV(stone[(y>>1)+3][a].h,s,v));
          if (frontback < 0) //Back
          engineQuad(-3<<ACCURACY-2,-3<<ACCURACY-2,0,
                     -3<<ACCURACY-2, 3<<ACCURACY-2,0,
                      3<<ACCURACY-2, 3<<ACCURACY-2,0,
                      3<<ACCURACY-2,-3<<ACCURACY-2,0,getHSV(stone[(y>>1)+3][a].h,s,v));
        }
        else
        {
          if (frontback > 0) //Front
            engineQuad(-3<<ACCURACY-2,-3<<ACCURACY-2,0,
                        3<<ACCURACY-2,-3<<ACCURACY-2,0,
                        3<<ACCURACY-2, 3<<ACCURACY-2,0,
                       -3<<ACCURACY-2, 3<<ACCURACY-2,0,getHSV(stone[(y>>1)+3][a].h,s,v));
          if (frontback < 0) //Back
            engineQuad(-3<<ACCURACY-2,-3<<ACCURACY-2,0,
                       -3<<ACCURACY-2, 3<<ACCURACY-2,0,
                        3<<ACCURACY-2, 3<<ACCURACY-2,0,
                        3<<ACCURACY-2,-3<<ACCURACY-2,0,getHSV(stone[(y>>1)+3][a].h,s,v));
        }      
      }
      memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);
    }
  //particles
  pparticle particle=firstparticle;
  while (particle)
  {
    memcpy(matrix,modellViewMatrix,sizeof(Sint32)*16);
    engineTranslate(particle->x,particle->y,particle->z);
    engineRotate(1<<ACCURACY,0,0,particle->rotx);
    engineRotate(0,1<<ACCURACY,0,particle->roty);
    engineRotate(0,0,1<<ACCURACY,particle->rotz);
    //the gp2x becomes old :-/
    if (settings_get_stone_quality() == 0)
      engineQuad(-(particle->age<<ACCURACY)/PARTICLE_AGE/4,
                 -(particle->age<<ACCURACY)/PARTICLE_AGE/4,0,
                  (particle->age<<ACCURACY)/PARTICLE_AGE/4,
                 -(particle->age<<ACCURACY)/PARTICLE_AGE/4,0,
                  (particle->age<<ACCURACY)/PARTICLE_AGE/4,
                  (particle->age<<ACCURACY)/PARTICLE_AGE/4,0,
                 -(particle->age<<ACCURACY)/PARTICLE_AGE/4,
                  (particle->age<<ACCURACY)/PARTICLE_AGE/4,0,
                  getHSV(particle->h,particle->s,particle->v));
    else
    {
      engineScale((particle->age<<ACCURACY)/PARTICLE_AGE/2,
                  (particle->age<<ACCURACY)/PARTICLE_AGE/2,
                  (particle->age<<ACCURACY)/PARTICLE_AGE/2);
      if (settings_get_stone_quality() == 2)
        drawMesh(stone_mesh,getHSV(particle->h,particle->s,particle->v));
      else
        drawMesh(stone_mesh_gp2x,getHSV(particle->h,particle->s,particle->v));
    }
    memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);
    particle=particle->next;
  }
    
  Sint32 r = 28<< ACCURACY-5;
  
    
  //engineEllipse(mycos(-(posx[0]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,posy[0],mysin((posx[0]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,4<<ACCURACY-3,4<<ACCURACY-3,getHSV((w*16)%(2*MY_PI),64,255));
  /*memcpy(matrix,modellViewMatrix,sizeof(Sint32)*16);
    engineTranslate(mycos(-(posx[0]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,posy[0],mysin((posx[0]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2);
    engineRotate(0,1<<ACCURACY,0,(posx[0]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1));
    engineRotate(0,0,1<<ACCURACY,mysin(w*32)/8);
    drawMesh(border_mesh,getHSV((w*16)%(2*MY_PI),64,255));
  memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);*/
  draw_particle(posx[0],posy[0],r,(SDL_GetTicks())%PARTICLE_SPEED,getSmallParticle());
  draw_particle(posx[0],posy[0],r,(SDL_GetTicks()+PARTICLE_SPEED/24)%PARTICLE_SPEED,getMiddleParticle());
  draw_particle(posx[0],posy[0],r,(SDL_GetTicks()+PARTICLE_SPEED/10)%PARTICLE_SPEED,getBigParticle());
    
  draw_particle(posx[0],posy[0],r,(SDL_GetTicks()+PARTICLE_SPEED/4)%PARTICLE_SPEED,getSmallParticle());
  draw_particle(posx[0],posy[0],r,(SDL_GetTicks()+PARTICLE_SPEED/4+PARTICLE_SPEED/24)%PARTICLE_SPEED,getMiddleParticle());
  draw_particle(posx[0],posy[0],r,(SDL_GetTicks()+PARTICLE_SPEED/4+PARTICLE_SPEED/10)%PARTICLE_SPEED,getBigParticle());
    
  draw_particle(posx[0],posy[0],r,(SDL_GetTicks()+PARTICLE_SPEED/2)%PARTICLE_SPEED,getSmallParticle());
  draw_particle(posx[0],posy[0],r,(SDL_GetTicks()+PARTICLE_SPEED/2+PARTICLE_SPEED/24)%PARTICLE_SPEED,getMiddleParticle());
  draw_particle(posx[0],posy[0],r,(SDL_GetTicks()+PARTICLE_SPEED/2+PARTICLE_SPEED/10)%PARTICLE_SPEED,getBigParticle());
    
  draw_particle(posx[0],posy[0],r,(SDL_GetTicks()+3*PARTICLE_SPEED/4)%PARTICLE_SPEED,getSmallParticle());
  draw_particle(posx[0],posy[0],r,(SDL_GetTicks()+3*PARTICLE_SPEED/4+PARTICLE_SPEED/24)%PARTICLE_SPEED,getMiddleParticle());
  draw_particle(posx[0],posy[0],r,(SDL_GetTicks()+3*PARTICLE_SPEED/4+PARTICLE_SPEED/10)%PARTICLE_SPEED,getBigParticle());
  //engineEllipse(mycos(-(pointposx[(  TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,pointposy[(  TIMEOUT-1+pointstart)%(2*TIMEOUT)],mysin((pointposx[(  TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,3<<ACCURACY-3,3<<ACCURACY-3,getHSV((w*16)%(2*MY_PI),64,255));
  engineDrawSurface(mycos(-(pointposx[(  TIMEOUT-1+pointstart+2*TIMEOUT-TIMEOUT/4)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,pointposy[(  TIMEOUT-1+pointstart+2*TIMEOUT-TIMEOUT/4)%(2*TIMEOUT)],mysin((pointposx[(  TIMEOUT-1+pointstart+2*TIMEOUT-TIMEOUT/4)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,getMiddleParticle());
  engineDrawSurface(mycos(-(pointposx[(  TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,pointposy[(  TIMEOUT-1+pointstart)%(2*TIMEOUT)],mysin((pointposx[(  TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,getMiddleParticle());
  engineDrawSurface(mycos(-(pointposx[(  TIMEOUT-1+pointstart+TIMEOUT/4)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,pointposy[(  TIMEOUT-1+pointstart+TIMEOUT/4)%(2*TIMEOUT)],mysin((pointposx[(  TIMEOUT-1+pointstart+TIMEOUT/4)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,getMiddleParticle());
  //engineEllipse(mycos(-(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],mysin((pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,2<<ACCURACY-3,2<<ACCURACY-3,getHSV((w*16)%(2*MY_PI),64,255));
  /*memcpy(matrix,modellViewMatrix,sizeof(Sint32)*16);
    engineTranslate(mycos(-(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2,pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],mysin((pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1)+MY_PI/2)*22>>2);
    engineRotate(0,1<<ACCURACY,0,(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)]>>HALF_ACCURACY+2)*(MY_PI>>HALF_ACCURACY+1));
    drawMesh(border_thin_mesh,getHSV((w*16)%(2*MY_PI),64,255));
  memcpy(modellViewMatrix,matrix,sizeof(Sint32)*16);*/
  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(SDL_GetTicks()+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(SDL_GetTicks()+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(SDL_GetTicks()+PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(SDL_GetTicks()+PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(SDL_GetTicks()+PARTICLE_SPEED2/2+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(SDL_GetTicks()+PARTICLE_SPEED2/2+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(SDL_GetTicks()+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/24)%PARTICLE_SPEED2,getSmallParticle());
  draw_particle2(pointposx[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],pointposy[(2*TIMEOUT-1+pointstart)%(2*TIMEOUT)],r,(SDL_GetTicks()+3*PARTICLE_SPEED2/4+PARTICLE_SPEED2/10)%PARTICLE_SPEED2,getMiddleParticle());

  
  engineDrawList();
  char buffer[256];
  SDL_Rect dstrect;
  SDL_Rect srcrect;
  //HUD on the left side
  SDL_Surface* shown = NULL;
  
  //setting "shown"
  shown = dummySurface;
  
  if (shown != NULL)
  {
      dstrect.x=1*engineWindowX/7-shown->w/2;
      dstrect.y=engineWindowY/2-shown->h/2;
      dstrect.w=shown->w;
      dstrect.h=shown->h;
      srcrect.x=0;
      srcrect.y=0;
      srcrect.w=dstrect.w;
      srcrect.h=dstrect.h;

      SDL_BlitSurface(shown, &srcrect,engineGetSurface(SURFACE_SURFACE), &dstrect);
  }
  
  //HUD on the right side
  drawtextMX(engineGetSurface(SURFACE_SURFACE),6*engineWindowX/7,1*engineWindowY/16,"Game Mode:",engineGetSurface(SURFACE_KEYMAP));
  if (mode & timeMode)
    sprintf(buffer,"Time Stole");
  else
    sprintf(buffer,"Points");
  drawtextMX(engineGetSurface(SURFACE_SURFACE),6*engineWindowX/7,2*engineWindowY/16,buffer,engineGetSurface(SURFACE_KEYMAP));
  if (mode & timeMode)
  {
    drawtextMX(engineGetSurface(SURFACE_SURFACE),6*engineWindowX/7,4*engineWindowY/16,"Time Past:",engineGetSurface(SURFACE_KEYMAP));
    sprintf(buffer,"%i Seconds",realTime/1000);
    drawtextMX(engineGetSurface(SURFACE_SURFACE),6*engineWindowX/7,5*engineWindowY/16,buffer,engineGetSurface(SURFACE_KEYMAP));
  }
  else
  {
    drawtextMX(engineGetSurface(SURFACE_SURFACE),6*engineWindowX/7,4*engineWindowY/16,"Points:",engineGetSurface(SURFACE_KEYMAP));
    sprintf(buffer,"%i",points);
    drawtextMX(engineGetSurface(SURFACE_SURFACE),6*engineWindowX/7,5*engineWindowY/16,buffer,engineGetSurface(SURFACE_KEYMAP));
  }

  dstrect.x=6*engineWindowX/7-getTimeSurface()->w/2;
  dstrect.y=13*engineWindowY/32;
  dstrect.w=gameTime*getTimeSurface()->w/START_TIME;
  dstrect.h=getTimeSurface()->h;
  srcrect.x=0;
  srcrect.y=0;
  srcrect.w=dstrect.w;
  srcrect.h=dstrect.h;

  SDL_BlitSurface(getTimeSurface(), &srcrect,engineGetSurface(SURFACE_SURFACE), &dstrect);

  sprintf(buffer,"%i",engineGetFps());
  drawtextMX(engineGetSurface(SURFACE_SURFACE),6*engineWindowX/7,15*engineWindowY/16,buffer,engineGetSurface(SURFACE_KEYMAP));

  draw_music();
  engineFlip();
}

int calc_game(Uint32 steps)
{
  if (wasResize())
    resize_particle(engineGetWindowX(),engineGetWindowY());
  pEngineInput engineInput = engineGetInput();
  if (engineInput->button[BUTTON_SELECT])
  {
    pause=!pause;
    engineInput->button[BUTTON_SELECT]=0;
  }
  if (pause)
    return 0;
  calc_music(steps);
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
      return 1;

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
          posx[0]-=(1<<ACCURACY)/TIMEOUT;
          if (posx[0]<0)
            posx[0]+=16<<ACCURACY;
          break;
        case 1:
          posx[0]+=(1<<ACCURACY)/TIMEOUT;
          if (posx[0]>=(16<<ACCURACY))
            posx[0]-=16<<ACCURACY;
          break;
        case 2:
          posy[0]-=(1<<ACCURACY)/(TIMEOUT/2);
          if (posy[0]<(-6<<ACCURACY))
            posy[0]=(-6<<ACCURACY);
          break;
        case 3:
          posy[0]+=(1<<ACCURACY)/(TIMEOUT/2);
          if (posy[0]>(6<<ACCURACY))
            posy[0]=(6<<ACCURACY);
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
        posx[0]=(posx[0]>>ACCURACY)<<ACCURACY;
        posy[0]=(posy[0]>>ACCURACY)<<ACCURACY;
        posx[3]=posx[0];
        posy[3]=posy[0];
        is_change=0;
      }
    }
    else
    {
      if (engineGetAxis(0)<0)
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
          is_change=posx[2]-posx[3]>>ACCURACY;
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
      if (engineGetAxis(0)>0)
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
          is_change=posx[2]-posx[3]>>ACCURACY;
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
      if (engineGetAxis(1)<0 && (((posy[0]>>ACCURACY)>-6 && direction!=3) || (direction==3 && (posy[2]>>ACCURACY)>-6)))
      {
        if (direction==3 && (posy[2]>>ACCURACY)>-6)
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
          is_change=posx[2]-posx[3]>>ACCURACY;
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
      if (engineGetAxis(1)>0 && (((posy[0]>>ACCURACY)< 6 && direction!=2) || (direction==2 && (posy[2]>>ACCURACY)< 6)))
      {
        if (direction==2 && (posy[2]>>ACCURACY)< 6)
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
          is_change=posx[2]-posx[3]>>ACCURACY;
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
      if (engineGetInput()->button[BUTTON_B] || engineGetInput()->button[BUTTON_A] ||
          engineGetInput()->button[BUTTON_X] || engineGetInput()->button[BUTTON_Y])
      {
        if (is_in_change((20-(posx[0]>>ACCURACY))%16,3+(posy[0]>>ACCURACY+1))==NULL &&
            is_in_change((20-(posx[2]>>ACCURACY))%16,3+(posy[2]>>ACCURACY+1))==NULL &&
            stone[3+(posy[0]>>ACCURACY+1)][(20-(posx[0]>>ACCURACY))%16].new==0 &&
            stone[3+(posy[2]>>ACCURACY+1)][(20-(posx[2]>>ACCURACY))%16].new==0 &&
            stone[3+(posy[0]>>ACCURACY+1)][(20-(posx[0]>>ACCURACY))%16].falling==0 &&
            stone[3+(posy[2]>>ACCURACY+1)][(20-(posx[2]>>ACCURACY))%16].falling==0 &&
            stone[3+(posy[0]>>ACCURACY+1)][(20-(posx[0]>>ACCURACY))%16].type>=0 &&
            stone[3+(posy[2]>>ACCURACY+1)][(20-(posx[2]>>ACCURACY))%16].type>=0)
        {
          play_switch();
          initate_change((20-(posx[0]>>ACCURACY))%16,3+(posy[0]>>ACCURACY+1),(20-(posx[2]>>ACCURACY))%16,3+(posy[2]>>ACCURACY+1));
        }
      }
    }
  }
    
  if (engineGetInput()->button[BUTTON_X])
  {
    engineGetInput()->button[BUTTON_X] = 0;
    change_music("Cosmic Conundrum","Nick May","CC BY-NC-ND");
  }
  
  w+=(steps*16)%(2*MY_PI);
  if (engineInput->button[BUTTON_START])
    return 1;
  return 0; 
}

void run_game(int playernumber_,GameMode mode_,int difficult_ /*0..9*/)
{
  playernumber = playernumber_;
  mode = mode_;
  difficult = difficult_;
  timeStep = difficult/2+2;
  engineLoop(draw_game,calc_game,10);
}
