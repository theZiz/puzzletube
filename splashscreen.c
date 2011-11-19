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
#include "splashscreen.h"

#define SPLASH_WAIT 2000

int splash_counter;
SDL_Surface *sparrow,*splash1,*splash2;

void draw_splash(void)
{
  spClearTarget(0);
  spResetZBuffer();
  spSetZSet(0);
  spSetZTest(0);
  
  SDL_Surface *mom = NULL;
  if (splash_counter>SPLASH_WAIT*2)
    mom = splash1;
  else
  if (splash_counter>SPLASH_WAIT && splash_counter<=SPLASH_WAIT*2)
    mom = splash2;
  else
    mom = sparrow;
  
  if (mom!=NULL)
    spBlitSurface(spGetWindowSurface()->w/2,spGetWindowSurface()->h/2,-1,mom);
  spFlip();  
}

int calc_splash(Uint32 steps)
{
  splash_counter -= steps;
  if (splash_counter <=0)
    return 1;
  PspInput engineInput = spGetInput();
  if ((engineInput->button[SP_BUTTON_START] ||
      engineInput->button[SP_BUTTON_A] || engineInput->button[SP_BUTTON_B] ||
      engineInput->button[SP_BUTTON_X] || engineInput->button[SP_BUTTON_Y]))
  {
    engineInput->button[SP_BUTTON_START] = 0;
    engineInput->button[SP_BUTTON_A] = 0;
    engineInput->button[SP_BUTTON_B] = 0;
    engineInput->button[SP_BUTTON_X] = 0;
    engineInput->button[SP_BUTTON_Y] = 0;
    splash_counter = (splash_counter/SPLASH_WAIT)*SPLASH_WAIT;
  }
  return 0;
}

void run_splashscreen(void (*resize)(Uint16 w,Uint16 h))
{
  if ((spGetSizeFactor()>>SP_ACCURACY) <= 1)
  {
    splash1 = IMG_Load("./images/splash_320.png");
    splash2 = IMG_Load("./images/sponsor_320.png");
  }
  else
  {
    splash1 = IMG_Load("./images/splash_800.png");
    splash2 = IMG_Load("./images/sponsor_800.png");
  }
  sparrow = IMG_Load("./images/sparrow.png");
  splash_counter = SPLASH_WAIT*3;
  spLoop(draw_splash,calc_splash,10,resize);
  SDL_FreeSurface(splash1);
  SDL_FreeSurface(sparrow);
  SDL_FreeSurface(splash2);
}

