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
#include "settings.h"

#define SPLASH_WAIT 2000
typedef SDL_Surface *PSDL_Surface;
int splash_counter;
Sint32 splash_rotation;
PSDL_Surface sparrow;

void draw_splash(void)
{
  spClearTarget(0);
  spResetZBuffer();
  spIdentity();
  spSetZSet(0);
  spSetZTest(0);
  spFontPointer font = settings_get_small_font();
  spBlitSurface(spGetWindowSurface()->w/2,spGetWindowSurface()->h/2-font->maxheight/2,-1,sparrow);
  spFontDrawMiddle(spGetWindowSurface()->w/2,spGetWindowSurface()->h/2+sparrow->h/2,-1,"Made  with  the  Sparrow3d  Engine  by  Ziz",font);
  spFlip();  	
}

int calc_splash(Uint32 steps)
{
  splash_rotation+=steps*32;
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
    return 1;
  }
  return 0;
}

void run_splashscreen(void (*resize)(Uint16 w,Uint16 h))
{
  splash_rotation = 0;
  sparrow = spLoadSurface("./images/sparrow.png");
  splash_counter = SPLASH_WAIT;
  spLoop(draw_splash,calc_splash,10,resize,NULL);
  SDL_FreeSurface(sparrow);
}

