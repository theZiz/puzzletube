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
#include "../sparrow3d/sparrow3d.h"
#include <SDL/SDL_image.h>
#include "particle.h"
#include "stars.h"

SDL_Surface* stars = NULL;

void init_stars()
{
  if (stars)
    SDL_FreeSurface(stars);
  SDL_Surface* dummy = SDL_CreateRGBSurface(SDL_SWSURFACE,spGetWindowSurface()->w*2,spGetWindowSurface()->h,16, 0 , 0 , 0 ,  0);
  SDL_FillRect(dummy,NULL,BACKGROUND_COLOR);
  
  SDL_Surface* star1 = IMG_Load("./images/star1.png");
  SDL_Surface* star2 = IMG_Load("./images/star2.png");
  SDL_Surface* star3 = IMG_Load("./images/star3.png");
  SDL_Surface* star4 = IMG_Load("./images/star4.png");
  SDL_Surface* star5 = IMG_Load("./images/star5.png");
  
  int i;
  //star1
  for (i = 0; i < sqrt((float)(dummy->w*dummy->h)/20000.0f); i++)
  {
    int x = rand()%dummy->w;
    int y = rand()%dummy->h;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = star1->w;
    dest.h = star1->h;
    SDL_BlitSurface(star1,NULL,dummy,&dest);
    if (x > dummy->w - star1->w)
    {
      dest.x = x-dummy->w;
      dest.y = y;
      dest.w = star1->w;
      dest.h = star1->h;
      SDL_BlitSurface(star1,NULL,dummy,&dest);
    }
  }
  //star2
  for (i = 0; i < sqrt((float)(dummy->w*dummy->h)/12000.0f); i++)
  {
    int x = rand()%dummy->w;
    int y = rand()%dummy->h;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = star2->w;
    dest.h = star2->h;
    SDL_BlitSurface(star2,NULL,dummy,&dest);
    if (x > dummy->w - star2->w)
    {
      dest.x = x-dummy->w;
      dest.y = y;
      dest.w = star2->w;
      dest.h = star2->h;
      SDL_BlitSurface(star2,NULL,dummy,&dest);
    }
  }
  //star3
  for (i = 0; i < (dummy->w*dummy->h)/7000; i++)
  {
    int x = rand()%dummy->w;
    int y = rand()%dummy->h;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = star3->w;
    dest.h = star3->h;
    SDL_BlitSurface(star3,NULL,dummy,&dest);
    if (x > dummy->w - star3->w)
    {
      dest.x = x-dummy->w;
      dest.y = y;
      dest.w = star3->w;
      dest.h = star3->h;
      SDL_BlitSurface(star3,NULL,dummy,&dest);
    }
  }
  //star4
  for (i = 0; i < (dummy->w*dummy->h)/4000; i++)
  {
    int x = rand()%dummy->w;
    int y = rand()%dummy->h;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = star4->w;
    dest.h = star4->h;
    SDL_BlitSurface(star4,NULL,dummy,&dest);
    if (x > dummy->w - star4->w)
    {
      dest.x = x-dummy->w;
      dest.y = y;
      dest.w = star4->w;
      dest.h = star4->h;
      SDL_BlitSurface(star4,NULL,dummy,&dest);
    }
  }
  //star5
  for (i = 0; i < (dummy->w*dummy->h)/2000; i++)
  {
    int x = rand()%dummy->w;
    int y = rand()%dummy->h;
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = star5->w;
    dest.h = star5->h;
    SDL_BlitSurface(star5,NULL,dummy,&dest);
    if (x > dummy->w - star5->w)
    {
      dest.x = x-dummy->w;
      dest.y = y;
      dest.w = star5->w;
      dest.h = star5->h;
      SDL_BlitSurface(star5,NULL,dummy,&dest);
    }
  }
  
  
  SDL_FreeSurface(star1);
  SDL_FreeSurface(star2);
  SDL_FreeSurface(star3);
  SDL_FreeSurface(star4);
  SDL_FreeSurface(star5);
    
  stars = SDL_DisplayFormat(dummy);
  
}


void draw_stars(Sint32 rotation)
{
  int from = ((rotation>>SP_HALF_ACCURACY) * spGetWindowSurface()->w / (SP_PI>>SP_HALF_ACCURACY)) % (spGetWindowSurface()->w*2);
  int to   = ((rotation>>SP_HALF_ACCURACY) * spGetWindowSurface()->w / (SP_PI>>SP_HALF_ACCURACY) + spGetWindowSurface()->w) % (spGetWindowSurface()->w*2);
  while (from < 0)
    from+=spGetWindowSurface()->w*2;
  while (to < 0)
    to+=spGetWindowSurface()->w*2;
  //printf("From %i to %i\n",from,to);
  if (from < to)
    spBlitSurfacePart(spGetWindowSurface()->w,spGetWindowSurface()->h/2,SP_MAX_NEGATIVE+1,stars,from,0,spGetWindowSurface()->w,spGetWindowSurface()->h);
  else
  {
    spBlitSurfacePart(spGetWindowSurface()->w,spGetWindowSurface()->h/2,SP_MAX_NEGATIVE+1,stars,from,0,spGetWindowSurface()->w-to,spGetWindowSurface()->h);
    spBlitSurfacePart(spGetWindowSurface()->w+spGetWindowSurface()->w-to,spGetWindowSurface()->h/2,SP_MAX_NEGATIVE+1,stars,0,0,to,spGetWindowSurface()->h);
  }
}
