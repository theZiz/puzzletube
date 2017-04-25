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
#include <sparrow3d.h>
#ifdef WIN32
	#include <SDL_image.h>
#else
	#include <SDL/SDL_image.h>
#endif
#include "particle.h"
#include "stars.h"

SDL_Surface* stars = NULL;

void init_stars()
{
  if (stars)
    SDL_FreeSurface(stars);
  stars = spCreateSurface(spGetWindowSurface()->w*2,spGetWindowSurface()->h);
  SDL_FillRect(stars,NULL,BACKGROUND_COLOR);
  
  SDL_Surface* star1 = IMG_Load("./images/star1.png");
  //if(!star1)
  //  printf("IMG_Load: %s\n", IMG_GetError());
  SDL_Surface* star2 = IMG_Load("./images/star2.png");
  //if(!star2)
  //  printf("IMG_Load: %s\n", IMG_GetError());
  SDL_Surface* star3 = IMG_Load("./images/star3.png");
  //if(!star3)
  //  printf("IMG_Load: %s\n", IMG_GetError());
  SDL_Surface* star4 = IMG_Load("./images/star4.png");
  //if(!star4)
  //  printf("IMG_Load: %s\n", IMG_GetError());
  SDL_Surface* star5 = IMG_Load("./images/star5.png");
  //if(!star5)
  //  printf("IMG_Load: %s\n", IMG_GetError());
  
  int i;
  //star1
  for (i = 0; i < sqrt((float)(stars->w*stars->h)/20000.0f); i++)
  {
    int x = rand()%stars->w;
    int y = rand()%stars->h;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = star1->w;
    dest.h = star1->h;
    SDL_BlitSurface(star1,NULL,stars,&dest);
    if (x > stars->w - star1->w)
    {
      dest.x = x-stars->w;
      dest.y = y;
      dest.w = star1->w;
      dest.h = star1->h;
      SDL_BlitSurface(star1,NULL,stars,&dest);
    }
  }
  //star2
  for (i = 0; i < sqrt((float)(stars->w*stars->h)/12000.0f); i++)
  {
    int x = rand()%stars->w;
    int y = rand()%stars->h;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = star2->w;
    dest.h = star2->h;
    SDL_BlitSurface(star2,NULL,stars,&dest);
    if (x > stars->w - star2->w)
    {
      dest.x = x-stars->w;
      dest.y = y;
      dest.w = star2->w;
      dest.h = star2->h;
      SDL_BlitSurface(star2,NULL,stars,&dest);
    }
  }
  //star3
  for (i = 0; i < (stars->w*stars->h)/7000; i++)
  {
    int x = rand()%stars->w;
    int y = rand()%stars->h;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = star3->w;
    dest.h = star3->h;
    SDL_BlitSurface(star3,NULL,stars,&dest);
    if (x > stars->w - star3->w)
    {
      dest.x = x-stars->w;
      dest.y = y;
      dest.w = star3->w;
      dest.h = star3->h;
      SDL_BlitSurface(star3,NULL,stars,&dest);
    }
  }
  //star4
  for (i = 0; i < (stars->w*stars->h)/4000; i++)
  {
    int x = rand()%stars->w;
    int y = rand()%stars->h;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = star4->w;
    dest.h = star4->h;
    SDL_BlitSurface(star4,NULL,stars,&dest);
    if (x > stars->w - star4->w)
    {
      dest.x = x-stars->w;
      dest.y = y;
      dest.w = star4->w;
      dest.h = star4->h;
      SDL_BlitSurface(star4,NULL,stars,&dest);
    }
  }
  //star5
  for (i = 0; i < (stars->w*stars->h)/2000; i++)
  {
    int x = rand()%stars->w;
    int y = rand()%stars->h;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = star5->w;
    dest.h = star5->h;
    SDL_BlitSurface(star5,NULL,stars,&dest);
    if (x > stars->w - star5->w)
    {
      dest.x = x-stars->w;
      dest.y = y;
      dest.w = star5->w;
      dest.h = star5->h;
      SDL_BlitSurface(star5,NULL,stars,&dest);
    }
  }
  
  
  SDL_FreeSurface(star1);
  SDL_FreeSurface(star2);
  SDL_FreeSurface(star3);
  SDL_FreeSurface(star4);
  SDL_FreeSurface(star5);
}


void draw_stars(Sint32 rotation)
{
  spSetHorizontalOrigin(SP_LEFT);
  int from = ((rotation>>SP_HALF_ACCURACY) * spGetWindowSurface()->w / (SP_PI>>SP_HALF_ACCURACY)) % (spGetWindowSurface()->w*2);
  int to   = ((rotation>>SP_HALF_ACCURACY) * spGetWindowSurface()->w / (SP_PI>>SP_HALF_ACCURACY) + spGetWindowSurface()->w) % (spGetWindowSurface()->w*2);
  while (from < 0)
    from+=spGetWindowSurface()->w*2;
  while (to < 0)
    to+=spGetWindowSurface()->w*2;
  //printf("From %i to %i\n",from,to);
  if (from < to)
    spBlitSurfacePart(0,spGetWindowSurface()->h/2,-spGetZFar()+1,stars,from,0,spGetWindowSurface()->w,spGetWindowSurface()->h);
  else
  {
    spBlitSurfacePart(0,spGetWindowSurface()->h/2,-spGetZFar()+1,stars,from,0,stars->w-from,spGetWindowSurface()->h);
    spBlitSurfacePart(stars->w-from,spGetWindowSurface()->h/2,-spGetZFar()+1,stars,0,0,to,spGetWindowSurface()->h);
  }
  spSetHorizontalOrigin(SP_CENTER);
}
