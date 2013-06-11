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
#include <sparrow3d.h>

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
  SDL_Surface* dummy = spCreateSurface(w,w);
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
  smallParticle = dummy;
  //middleParticle 3.5% of y
  if (middleParticle)
    SDL_FreeSurface(middleParticle);
  w = winY*7/200;
  if (w&1)
    w++;
  dummy = spCreateSurface(w,w);
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
  middleParticle = dummy;
  //bigParticle 5% of y
  if (bigParticle)
    SDL_FreeSurface(bigParticle);
  w = winY*5/100;
  if (w&1)
    w++;
  dummy = spCreateSurface(w,w);
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
  bigParticle = dummy;
  //timeSurface
  if (timeSurface)
    SDL_FreeSurface(timeSurface);
  w = winY*2/6;
  if (w&1)
    w++;
  dummy = spCreateSurface(w,w/8);
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
      if ((x == 0) ^ (y == 0) ^ (x == dummy->w-1) ^ (y == dummy->h-1))
        pixel[x+y*dummy->w] = 65535;
      else
      if ((x == 0) || (y == 0) || (x == dummy->w-1) || (y == dummy->h-1))
        pixel[x+y*dummy->w] = SP_ALPHA_COLOR;
      else
      {
        int g = (x*255/dummy->w)>>2;
        int r = (255-x*255/dummy->w)>>3;
        pixel[x+y*dummy->w] = (r<<11)+(g<<5);
      }
    }
  SDL_UnlockSurface(dummy);
  timeSurface = dummy;
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
	spSetAlphaPattern4x4(PARTICLE_ALPHA,0);
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
  spDeactivatePattern();
}  

void draw_border(int x1,int y1,int x2,int y2,Uint16 color)
{
  SDL_LockSurface(spGetWindowSurface());
  Uint16* pixel = (Uint16*)spGetWindowSurface()->pixels;
  int x,y;
  int r = 6 * spGetSizeFactor() >> SP_ACCURACY;
  spHorizentalLine(pixel,x1+r,y1,x2-x1-2*r,color,1,spGetWindowSurface()->w,spGetWindowSurface()->h);
  spHorizentalLine(pixel,x1+r,y2,x2-x1-2*r,color,1,spGetWindowSurface()->w,spGetWindowSurface()->h);
  for (x = x1; x<=x2; x++)
  {
    //Jumping over emptiness:
    if (x-x1 == r)
      x = (x2-r);
    for (y = y1; y<=y2; y++)
    if (x >= 0 && y >= 0 && x<spGetWindowSurface()->w && y<spGetWindowSurface()->h)
    {
      //left, top edge
      if (x-x1 < r && y-y1 < r)
      {
        int value = (r-x+x1)*(r-x+x1)+(r-y+y1)*(r-y+y1);
        if ( value > (r-1)*(r-1) && value < r*r)
          pixel[x+y*spGetWindowSurface()->w] = color;
      }
      else
      //right, top edge
      if (x2-x < r && y-y1 < r)
      {
        int value = (r-x2+x)*(r-x2+x)+(r-y+y1)*(r-y+y1);
        if ( value > (r-1)*(r-1) && value < r*r)
          pixel[x+y*spGetWindowSurface()->w] = color;
      }
      else
      //left, bottom edge
      if (x-x1 < r && y2-y < r)
      {
        int value = (r-x+x1)*(r-x+x1)+(r-y2+y)*(r-y2+y);
        if ( value > (r-1)*(r-1) && value < r*r)
          pixel[x+y*spGetWindowSurface()->w] = color;
      }
      else
      //right, bottom edge
      if (x2-x < r && y2-y < r)
      {
        int value = (r-x2+x)*(r-x2+x)+(r-y2+y)*(r-y2+y);
        if ( value > (r-1)*(r-1) && value < r*r)
          pixel[x+y*spGetWindowSurface()->w] = color;
      }
      else
      if (x == x1 | y == y1 | x == x2 | y == y2)
        pixel[x+y*spGetWindowSurface()->w] = color;
      
    }
  } 
  
  SDL_UnlockSurface(spGetWindowSurface());
}

void draw_filled_border(int x1,int y1,int x2,int y2,Uint16 color)
{
  SDL_LockSurface(spGetWindowSurface());
  Uint16* pixel = (Uint16*)spGetWindowSurface()->pixels;
  int x,y;
  int r = 6 * spGetSizeFactor() >> SP_ACCURACY;
  for (x = x1; x<=x2; x++)
    for (y = y1; y<=y2; y++)
    if (x >= 0 && y >= 0 && x<spGetWindowSurface()->w && y<spGetWindowSurface()->h)
    {
      /*//left, top edge
      if (x-x1 < r && y-y1 < r)
      {
        int value = (r-x+x1)*(r-x+x1)+(r-y+y1)*(r-y+y1);
        if (value < r*r && ((x+(y&3))&3))
          pixel[x+y*spGetWindowSurface()->w] = color;
      }
      else
      //right, top edge
      if (x2-x < r && y-y1 < r)
      {
        int value = (r-x2+x)*(r-x2+x)+(r-y+y1)*(r-y+y1);
        if (value < r*r && ((x+(y&3))&3))
          pixel[x+y*spGetWindowSurface()->w] = color;
      }
      else
      //left, bottom edge
      if (x-x1 < r && y2-y < r)
      {
        int value = (r-x+x1)*(r-x+x1)+(r-y2+y)*(r-y2+y);
        if (value < r*r && ((x+(y&3))&3))
          pixel[x+y*spGetWindowSurface()->w] = color;
      }
      else
      //right, bottom edge
      if (x2-x < r && y2-y < r)
      {
        int value = (r-x2+x)*(r-x2+x)+(r-y2+y)*(r-y2+y);
        if (value < r*r && ((x+(y&3))&3))
          pixel[x+y*spGetWindowSurface()->w] = color;
      }
      else*/
      if ((x+(y&3))&3)
        pixel[x+y*spGetWindowSurface()->w] = color;
      
    }
    
  
  SDL_UnlockSurface(spGetWindowSurface());
}
