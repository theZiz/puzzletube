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
#include "particle.h"

SDL_Surface* smallParticle = NULL;
SDL_Surface* middleParticle = NULL;
SDL_Surface* bigParticle = NULL;

#define NAMELESSCONSTANT 512.0
#define ALPHA_FACTOR 0.9

void resize_particle(int winX,int winY)
{
  //smallParticle 2% of y
  if (smallParticle)
    SDL_FreeSurface(smallParticle);
  SDL_Surface* dummy = SDL_CreateRGBSurface(SDL_SWSURFACE,winY*2/100,
            winY*2/100,32,0xff000000,0x00ff0000,0x0000ff00,0x000000ff);
  SDL_LockSurface(dummy);
  Uint8* pixel = dummy->pixels;
  int x,y;
  for (x = 0; x < dummy->w; x++)
    for (y = 0; y < dummy->h; y++)
    {
      int alpha = (int)(sqrt((float)(dummy->w*dummy->w)/4.0-
          ((float)(x)-(float)(dummy->w)/2.0)*
          ((float)(x)-(float)(dummy->w)/2.0)-
          ((float)(y)-(float)(dummy->h)/2.0)*
          ((float)(y)-(float)(dummy->h)/2.0))/(float)(dummy->h)*NAMELESSCONSTANT*ALPHA_FACTOR);
      if (alpha<255)
        pixel[(x+y*dummy->w)*4+0] = alpha;
      else 
        pixel[(x+y*dummy->w)*4+0] = 255;
      pixel[(x+y*dummy->w)*4+1] = 255;
      pixel[(x+y*dummy->w)*4+2] = 255;
      pixel[(x+y*dummy->w)*4+3] = 255;      
    }
  SDL_UnlockSurface(dummy);
  smallParticle = SDL_DisplayFormatAlpha(dummy);
  SDL_FreeSurface(dummy);
  //middleParticle 3.5% of y
  if (middleParticle)
    SDL_FreeSurface(middleParticle);
  dummy = SDL_CreateRGBSurface(SDL_SWSURFACE,winY*7/200,
            winY*7/200,32,0xff000000,0x00ff0000,0x0000ff00,0x000000ff);
  SDL_LockSurface(dummy);
  pixel = dummy->pixels;
  for (x = 0; x < dummy->w; x++)
    for (y = 0; y < dummy->h; y++)
    {
      int alpha = (int)(sqrt((float)(dummy->w*dummy->w)/4.0-
          ((float)(x)-(float)(dummy->w)/2.0)*
          ((float)(x)-(float)(dummy->w)/2.0)-
          ((float)(y)-(float)(dummy->h)/2.0)*
          ((float)(y)-(float)(dummy->h)/2.0))/(float)(dummy->h)*NAMELESSCONSTANT*ALPHA_FACTOR);
      if (alpha<255)
        pixel[(x+y*dummy->w)*4+0] = alpha;
      else 
        pixel[(x+y*dummy->w)*4+0] = 255;
      pixel[(x+y*dummy->w)*4+1] = 255;
      pixel[(x+y*dummy->w)*4+2] = 255;
      pixel[(x+y*dummy->w)*4+3] = 255;      
    }
  SDL_UnlockSurface(dummy);
  middleParticle = SDL_DisplayFormatAlpha(dummy);
  SDL_FreeSurface(dummy);
  //bigParticle 5% of y
  if (bigParticle)
    SDL_FreeSurface(bigParticle);
  dummy = SDL_CreateRGBSurface(SDL_SWSURFACE,winY*5/100,
            winY*5/100,32,0xff000000,0x00ff0000,0x0000ff00,0x000000ff);
  SDL_LockSurface(dummy);
  pixel = dummy->pixels;
  for (x = 0; x < dummy->w; x++)
    for (y = 0; y < dummy->h; y++)
    {
      int alpha = (int)(sqrt((float)(dummy->w*dummy->w)/4.0-
          ((float)(x)-(float)(dummy->w)/2.0)*
          ((float)(x)-(float)(dummy->w)/2.0)-
          ((float)(y)-(float)(dummy->h)/2.0)*
          ((float)(y)-(float)(dummy->h)/2.0))/(float)(dummy->h)*NAMELESSCONSTANT*ALPHA_FACTOR);
      if (alpha<255)
        pixel[(x+y*dummy->w)*4+0] = alpha;
      else 
        pixel[(x+y*dummy->w)*4+0] = 255;
      pixel[(x+y*dummy->w)*4+1] = 255;
      pixel[(x+y*dummy->w)*4+2] = 255;
      pixel[(x+y*dummy->w)*4+3] = 255;      
    }
  SDL_UnlockSurface(dummy);
  bigParticle = SDL_DisplayFormatAlpha(dummy);
  SDL_FreeSurface(dummy);
}

SDL_Surface* getSmallParticle()
{
  return smallParticle;
}

SDL_Surface* getMiddleParticle()
{
  return middleParticle;
}

SDL_Surface* getBigParticle()
{
  return bigParticle;
}
