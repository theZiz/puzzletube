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
#include "../sparrow3d/sparrow3d.h"

SDL_Surface* smallParticle = NULL;
SDL_Surface* middleParticle = NULL;
SDL_Surface* bigParticle = NULL;
SDL_Surface* timeSurface = NULL;

int particle_mode = 1;

#define NAMELESSCONSTANT 512.0
#define ALPHA_FACTOR 0.9

void set_particle_mode(int mode)
{
  particle_mode = mode;
}

void resize_particle(int winX,int winY)
{
  //smallParticle 2% of y
  if (smallParticle)
    SDL_FreeSurface(smallParticle);
  int w = winY*2/100;
  if (w&1)
    w++;
  SDL_Surface* dummy = SDL_CreateRGBSurface(SDL_SWSURFACE,w,w,16,0,0,0,0);
  SDL_LockSurface(dummy);
  Uint16* pixel = dummy->pixels;
  int x,y;
  for (x = 0; x < dummy->w; x++)
    for (y = 0; y < dummy->h; y++)
    {
      int X = dummy->w/2-x;
      int Y = dummy->h/2-y;
      int R = dummy->w/2;
      if (X*X+Y*Y < R*R)
        pixel[x+y*dummy->w] = 65535;
      else
        pixel[x+y*dummy->w] = SP_ALPHA_COLOR;
    }
  SDL_UnlockSurface(dummy);
  smallParticle = SDL_DisplayFormat(dummy);
  SDL_FreeSurface(dummy);
  //middleParticle 3.5% of y
  if (middleParticle)
    SDL_FreeSurface(middleParticle);
  w = winY*7/200;
  if (w&1)
    w++;
  dummy = SDL_CreateRGBSurface(SDL_SWSURFACE,w,w,16,0,0,0,0);
  SDL_LockSurface(dummy);
  pixel = dummy->pixels;
  for (x = 0; x < dummy->w; x++)
    for (y = 0; y < dummy->h; y++)
    {
      int X = dummy->w/2-x;
      int Y = dummy->h/2-y;
      int R = dummy->w/2;
      if (X*X+Y*Y < R*R)
        pixel[x+y*dummy->w] = 65535;
      else
        pixel[x+y*dummy->w] = SP_ALPHA_COLOR;
    }
  SDL_UnlockSurface(dummy);
  middleParticle = SDL_DisplayFormat(dummy);
  SDL_FreeSurface(dummy);
  //bigParticle 5% of y
  if (bigParticle)
    SDL_FreeSurface(bigParticle);
  w = winY*5/100;
  if (w&1)
    w++;
  dummy = SDL_CreateRGBSurface(SDL_SWSURFACE,w,w,16,0,0,0,0);
  SDL_LockSurface(dummy);
  pixel = dummy->pixels;
  for (x = 0; x < dummy->w; x++)
    for (y = 0; y < dummy->h; y++)
    {
      int X = dummy->w/2-x;
      int Y = dummy->h/2-y;
      int R = dummy->w/2;
      if (X*X+Y*Y < R*R)
        pixel[x+y*dummy->w] = 65535;
      else
        pixel[x+y*dummy->w] = SP_ALPHA_COLOR;
    }
  SDL_UnlockSurface(dummy);
  bigParticle = SDL_DisplayFormat(dummy);
  SDL_FreeSurface(dummy);
  //timeSurface
  if (timeSurface)
    SDL_FreeSurface(timeSurface);
  dummy = SDL_CreateRGBSurface(SDL_SWSURFACE,winY*2/6,winY*3/100,16,0,0,0,0);
  SDL_LockSurface(dummy);
  pixel = dummy->pixels;
  //memset(pixel,255,dummy->w*dummy->h*4);
  for (x = 0; x < dummy->w; x++)
    for (y = 0; y < dummy->h; y++)
    {
      /*pixel[(x+y*dummy->w)*4+0] = 255;
      if (x < dummy->w/2)
        pixel[(x+y*dummy->w)*4+1] = 2*x*192/dummy->w;
      else
        pixel[(x+y*dummy->w)*4+1] = 2*(dummy->w-x)*192/dummy->w;
      pixel[(x+y*dummy->w)*4+2] = x*255/dummy->w;
      pixel[(x+y*dummy->w)*4+3] = 255-x*255/dummy->w;      */
      pixel[x+y*dummy->w] = 65535;
    }
  SDL_UnlockSurface(dummy);
  timeSurface = SDL_DisplayFormat(dummy);
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

SDL_Surface* getTimeSurface()
{
  return timeSurface;
}

void draw_particle_circle(int direction,int counter)
{
  if (direction == -1)
  {
    //Left Circle
    spBlit3D(+spCos(counter*700+5*SP_PI/10),spSin(counter*700+5*SP_PI/10),0,getBigParticle());
    spBlit3D(+spCos(counter*700+3*SP_PI/10),spSin(counter*700+3*SP_PI/10),0,getMiddleParticle());
    spBlit3D(+spCos(counter*700+2*SP_PI/10),spSin(counter*700+2*SP_PI/10),0,getMiddleParticle());
    spBlit3D(+spCos(counter*700+1*SP_PI/10),spSin(counter*700+1*SP_PI/10),0,getSmallParticle());
    spBlit3D(+spCos(counter*700+0*SP_PI/10),spSin(counter*700+0*SP_PI/10),0,getSmallParticle());
    
    spBlit3D(+spCos(counter*700+15*SP_PI/10),spSin(counter*700+15*SP_PI/10),0,getBigParticle());
    spBlit3D(+spCos(counter*700+13*SP_PI/10),spSin(counter*700+13*SP_PI/10),0,getMiddleParticle());
    spBlit3D(+spCos(counter*700+12*SP_PI/10),spSin(counter*700+12*SP_PI/10),0,getMiddleParticle());
    spBlit3D(+spCos(counter*700+11*SP_PI/10),spSin(counter*700+11*SP_PI/10),0,getSmallParticle());
    spBlit3D(+spCos(counter*700+10*SP_PI/10),spSin(counter*700+10*SP_PI/10),0,getSmallParticle());
  }
  if (direction == 1)
  {  
    //Right circle
    spBlit3D(-spCos(counter*700+5*SP_PI/10),spSin(counter*700+5*SP_PI/10),0,getBigParticle());
    spBlit3D(-spCos(counter*700+3*SP_PI/10),spSin(counter*700+3*SP_PI/10),0,getMiddleParticle());
    spBlit3D(-spCos(counter*700+2*SP_PI/10),spSin(counter*700+2*SP_PI/10),0,getMiddleParticle());
    spBlit3D(-spCos(counter*700+1*SP_PI/10),spSin(counter*700+1*SP_PI/10),0,getSmallParticle());
    spBlit3D(-spCos(counter*700+0*SP_PI/10),spSin(counter*700+0*SP_PI/10),0,getSmallParticle());

    spBlit3D(-spCos(counter*700+15*SP_PI/10),spSin(counter*700+15*SP_PI/10),0,getBigParticle());
    spBlit3D(-spCos(counter*700+13*SP_PI/10),spSin(counter*700+13*SP_PI/10),0,getMiddleParticle());
    spBlit3D(-spCos(counter*700+12*SP_PI/10),spSin(counter*700+12*SP_PI/10),0,getMiddleParticle());
    spBlit3D(-spCos(counter*700+11*SP_PI/10),spSin(counter*700+11*SP_PI/10),0,getSmallParticle());
    spBlit3D(-spCos(counter*700+10*SP_PI/10),spSin(counter*700+10*SP_PI/10),0,getSmallParticle());
  }
}  
