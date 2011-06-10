#ifndef _GAME_H
#define _GAME_H

#include "../3dengine/3dengine.h"
#include "../3dengine/meshloader.h"
#include "particle.h"

#define TIMEOUT 200
#define FALL_TIME TIMEOUT
#define NEW_TIME (TIMEOUT/2)
#define CHANGE_TIME TIMEOUT
#define MAX_TYPES 6
#define PARTICLE_AGE 1000

typedef enum GameMode
{
  timeMode
} GameMode;

typedef struct sstone *pstone;
typedef struct sstone {
  Sint32 h;
  Uint8 s,v;
  signed char type;
  char inmovement;
  int falling;
  int new;
} tstone;

typedef struct swinsituation *pwinsituation;
typedef struct swinsituation {
  int x,y;
  pwinsituation next;
} twinsituation;

typedef struct schange *pchange;
typedef struct schange {
  int progress;
  int ax,ay,bx,by;
  pchange next;
} tchange;

typedef struct sparticle *pparticle;
typedef struct sparticle {
  Sint32 x,y,z;
  Sint32 dx,dy,dz;
  Sint32 h;
  Uint8 s,v;
  Sint32 rotx,roty,rotz;
  int age;
  pparticle next;
} tparticle;


void run_game(int playernumber,GameMode mode,int difficult /*0..9*/);

#endif
