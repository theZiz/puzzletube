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
#ifndef _GAME_H
#define _GAME_H

#include "../3dengine/3dengine.h"
#include "../3dengine/meshloader.h"
#include "particle.h"
#include "stars.h"

#define TIMEOUT 200
#define FALL_TIME TIMEOUT
#define NEW_TIME (TIMEOUT/2)
#define CHANGE_TIME TIMEOUT
#define MAX_TYPES 6
#define PARTICLE_AGE 1000

typedef enum GameMode
{
  timeMode = 1, //opposite is pointMode
  removedBlackMode = 2,
  randomQuestMode = 4,
  colorRemoveMode = 8,
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
void prepare_game_objects(char complete,int colornumber_);
void delete_game_objects();

#endif
