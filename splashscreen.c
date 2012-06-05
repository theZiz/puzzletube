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
typedef SDL_Surface *PSDL_Surface;
int splash_counter;
Sint32 splash_rotation;
SDL_Surface *splash1,*splash2;
PSDL_Surface sparrow;//[6];

void draw_splash(void)
{
  spClearTarget(0);
  spResetZBuffer();
  spIdentity();
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
  /*else
  {
    spSetZSet(1);
    spSetZTest(1);
    spTranslate(0,0,(-13<<SP_ACCURACY-1));
    spRotateX(splash_rotation);
    spRotateY(splash_rotation);
    spRotateZ(splash_rotation);
    //Front / Back
    spBindTexture(sparrow[0]);
    spQuadTex3D(-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,sparrow[0]->w-SP_FONT_EXTRASPACE-1,sparrow[0]->h-SP_FONT_EXTRASPACE-1,
                -3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,sparrow[0]->w-SP_FONT_EXTRASPACE-1,SP_FONT_EXTRASPACE,
                 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,SP_FONT_EXTRASPACE,
                 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,sparrow[0]->h-SP_FONT_EXTRASPACE-1,65535);
    spBindTexture(sparrow[3]);
    spQuadTex3D( 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,sparrow[0]->w-SP_FONT_EXTRASPACE-1,sparrow[0]->h-SP_FONT_EXTRASPACE-1,
                 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,sparrow[0]->w-SP_FONT_EXTRASPACE-1,SP_FONT_EXTRASPACE,
                -3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,SP_FONT_EXTRASPACE,
                -3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,sparrow[0]->h-SP_FONT_EXTRASPACE-1,65535);
    //Left / Right
    spBindTexture(sparrow[1]);
    spQuadTex3D(-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,sparrow[0]->w-SP_FONT_EXTRASPACE-1,sparrow[0]->h-SP_FONT_EXTRASPACE-1,
                -3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,sparrow[0]->w-SP_FONT_EXTRASPACE-1,SP_FONT_EXTRASPACE,
                -3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,SP_FONT_EXTRASPACE,
                -3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,sparrow[0]->h-SP_FONT_EXTRASPACE-1,65535);
    spBindTexture(sparrow[4]);
    spQuadTex3D( 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,sparrow[0]->w-SP_FONT_EXTRASPACE-1,sparrow[0]->h-SP_FONT_EXTRASPACE-1,
                 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,sparrow[0]->w-SP_FONT_EXTRASPACE-1,SP_FONT_EXTRASPACE,
                 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,SP_FONT_EXTRASPACE,
                 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,sparrow[0]->h-SP_FONT_EXTRASPACE-1,65535);
    //Up / Down
    spBindTexture(sparrow[2]);
    spQuadTex3D( 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,sparrow[0]->w-SP_FONT_EXTRASPACE-1,sparrow[0]->h-SP_FONT_EXTRASPACE-1,
                 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,sparrow[0]->w-SP_FONT_EXTRASPACE-1,SP_FONT_EXTRASPACE,
                -3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,SP_FONT_EXTRASPACE,
                -3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,sparrow[0]->h-SP_FONT_EXTRASPACE-1,65535);
    spBindTexture(sparrow[5]);
    spQuadTex3D(-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,sparrow[0]->w-SP_FONT_EXTRASPACE-1,sparrow[0]->h-SP_FONT_EXTRASPACE-1,
                -3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,sparrow[0]->w-SP_FONT_EXTRASPACE-1,SP_FONT_EXTRASPACE,
                 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,SP_FONT_EXTRASPACE,
                 3<<SP_ACCURACY-1,-3<<SP_ACCURACY-1, 3<<SP_ACCURACY-1,SP_FONT_EXTRASPACE,sparrow[0]->h-SP_FONT_EXTRASPACE-1,65535);
  }*/
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
    splash_counter = (splash_counter/SPLASH_WAIT)*SPLASH_WAIT;
  }
  return 0;
}

void run_splashscreen(void (*resize)(Uint16 w,Uint16 h))
{
  splash_rotation = 0;
  if ((spGetSizeFactor()>>SP_ACCURACY) <= 1)
  {
    splash1 = spLoadSurface("./images/splash_320.png");
    splash2 = spLoadSurface("./images/sponsor_320.png");
  }
  else
  {
    splash1 = spLoadSurface("./images/splash_800.png");
    splash2 = spLoadSurface("./images/sponsor_800.png");
  }
  sparrow = spLoadSurface("./images/sparrow.png");
  /*sparrow[0] = spLoadSurface("./images/sparrow_1.png");
  sparrow[1] = spLoadSurface("./images/sparrow_2.png");
  sparrow[2] = spLoadSurface("./images/sparrow_3.png");
  sparrow[3] = spLoadSurface("./images/sparrow_4.png");
  sparrow[4] = spLoadSurface("./images/sparrow_5.png");
  sparrow[5] = spLoadSurface("./images/sparrow_6.png");*/
  splash_counter = SPLASH_WAIT*3;
  spLoop(draw_splash,calc_splash,10,resize,NULL);
  SDL_FreeSurface(splash1);
  SDL_FreeSurface(splash2);
  /*SDL_FreeSurface(sparrow[0]);
  SDL_FreeSurface(sparrow[1]);
  SDL_FreeSurface(sparrow[2]);
  SDL_FreeSurface(sparrow[3]);
  SDL_FreeSurface(sparrow[4]);
  SDL_FreeSurface(sparrow[5]);*/
  SDL_FreeSurface(sparrow);
}

