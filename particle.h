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
#ifndef _PARTICLE_H
#define _PARTICLE_H

#include <SDL/SDL.h>
#include <math.h>

#define PARTICLE_ALPHA 140

SDL_Surface* getSmallParticle();
SDL_Surface* getMiddleParticle();
SDL_Surface* getBigParticle();
SDL_Surface* getTimeSurface();
void resize_particle(int winX,int winY);
void set_particle_mode(int mode); //1 good, 0 bad
void draw_particle_circle(int direction,int counter);
void draw_border(int x1,int y1,int x2,int y2,Uint16 color);
void draw_filled_border(int x1,int y1,int x2,int y2,Uint16 color);
#endif
