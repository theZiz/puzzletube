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
SDL_Surface* splash1,*splash2;

void draw_splash(void)
{
  engineClearScreen(0);
  
  SDL_Surface *mom = NULL;
  if (splash_counter>SPLASH_WAIT)
    mom = splash1;
  else
    mom = splash2;
  
  if (mom!=NULL)
  {
    SDL_Rect dstrect;
    SDL_Rect srcrect;
    dstrect.x=engineGetWindowX()/2-mom->w/2;
    dstrect.y=engineGetWindowY()/2-mom->h/2;
    dstrect.w=mom->w;
    dstrect.h=mom->h;
    srcrect.x=0;
    srcrect.y=0;
    srcrect.w=dstrect.w;
    srcrect.h=dstrect.h;

    SDL_BlitSurface(mom, &srcrect,engineGetSurface(SURFACE_SURFACE), &dstrect);
  }
  engineFlip();  
}

int calc_splash(Uint32 steps)
{
  splash_counter -= steps;
  if (splash_counter <=0)
    return 1;
  pEngineInput engineInput = engineGetInput();
  if ((engineInput->button[BUTTON_START] ||
      engineInput->button[BUTTON_A] || engineInput->button[BUTTON_B] ||
      engineInput->button[BUTTON_X] || engineInput->button[BUTTON_Y]))
  {
    engineInput->button[BUTTON_START] = 0;
    engineInput->button[BUTTON_A] = 0;
    engineInput->button[BUTTON_B] = 0;
    engineInput->button[BUTTON_X] = 0;
    engineInput->button[BUTTON_Y] = 0;
    splash_counter = (splash_counter/SPLASH_WAIT)*SPLASH_WAIT;
  }
  return 0;
}

void run_splashscreen()
{
  int globalzoom=min(engineGetWindowX()/320,engineGetWindowY()/240);
  if (globalzoom <= 1)
  {
    splash1 = IMG_Load("./images/splash_320.png");
    splash2 = IMG_Load("./images/sponsor_320.png");
  }
  else
  {
    splash1 = IMG_Load("./images/splash_800.png");
    splash2 = IMG_Load("./images/sponsor_800.png");
  }
  splash_counter = SPLASH_WAIT*2;
  engineLoop(draw_splash,calc_splash,10);
  SDL_FreeSurface(splash1);
  SDL_FreeSurface(splash2);
}

