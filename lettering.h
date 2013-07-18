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

#ifndef _LETTERING_H
#define _LETTERING_H

#include <SDL.h>
#include <SDL_image.h>
#include <sparrow3d.h>
#include "game.h"

#define LETTERING_TIMEOUT 10000
#define BORDERING_TIMEOUT 2000

typedef struct slettering *plettering;
typedef struct slettering {
  char* text;
  spFontPointer font;
  int age;
  int y;
  plettering next;
} tlettering;

void add_lettering(char* text,spFontPointer font);
void add_line();
void calc_lettering(int steps);
void delete_all_lettering();
plettering get_first_lettering();

typedef struct sbordering *pbordering;
typedef struct sbordering {
  Sint32 horizental_line[8][16];
  Sint32 vertical_line[7][16];
  int age;
  Sint32 alpha;
  pbordering next;
} tbordering;

void add_bordering(pwinsituation winsituation,Uint16 color);
void calc_bordering(int steps);
void delete_all_bordering();
pbordering get_first_bordering();


#endif
