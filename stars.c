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
#include "../sparrow3d/3dengine.h"
#include "particle.h"
#include "stars.h"

tpoint star[STAR_COUNT];
char starkind[STAR_COUNT];

void init_stars()
{
  int a;
  for (a=0;a<STAR_COUNT;a++)
  {
    Sint32 angle = rand()%(2*MY_PI);
    star[a].x = spCos(angle)*20;
    star[a].y = rand()%(40<<ACCURACY)-(20<<ACCURACY);
    star[a].z = spSin(angle)*20;
    starkind[a] = rand()%10;
  }
}


void draw_stars()
{
  int a;
  for (a=0;a<STAR_COUNT;a++)
  {
    spBlit3D(star[a].x,star[a].y,star[a].z,getSmallParticle());
  }
}
