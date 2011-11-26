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

#include "lettering.h"
#include <stdlib.h>
#include <string.h>

plettering first_lettering = NULL;

void add_lettering(char* text,spFontPointer font)
{
  //Create
  plettering lettering = (plettering)malloc(sizeof(tlettering));
  lettering->text = (char*)malloc(strlen(text)+1);
  sprintf(lettering->text,"%s",text);
  lettering->age = LETTERING_TIMEOUT;
  lettering->y = spGetWindowSurface()->h/20;
  lettering->font = font;
  lettering->next = first_lettering;
  first_lettering = lettering;
  
  //Recalculate the y-values
  lettering = first_lettering->next;
  while (lettering)
  {
    lettering->y += first_lettering->font->maxheight;
    lettering = lettering->next;
  }
}

void add_line()
{
  //Create
  plettering lettering = (plettering)malloc(sizeof(tlettering));
  lettering->text = NULL;
  lettering->age = LETTERING_TIMEOUT;
  lettering->y = spGetWindowSurface()->h/16;
  lettering->font = NULL;
  lettering->next = first_lettering;
  first_lettering = lettering;
  
  //Recalculate the y-values
  lettering = first_lettering->next;
  while (lettering)
  {
    lettering->y += spGetWindowSurface()->h/32;
    lettering = lettering->next;
  }
}

void calc_lettering(int steps)
{
  plettering lettering = first_lettering;
  plettering last_lettering = NULL;
  //Adding
  while (lettering)
  {
    lettering->age -= steps;
    if (lettering->age<=0 || lettering->y > spGetWindowSurface()->h)
      break;
    last_lettering = lettering;
    lettering = lettering->next;
  }
  //Deleting if too old or to low (y)
  while (lettering)
  {
    plettering next = lettering->next;
    if (lettering->text)
      free(lettering->text);
    free(lettering);
    lettering = next;
  }
  if (last_lettering)
    last_lettering->next = NULL;
  else
    first_lettering = NULL;
}

void delete_all_lettering()
{
  while (first_lettering)
  {
    plettering next = first_lettering->next;
    if (first_lettering->text)
      free(first_lettering->text);
    free(first_lettering);
    first_lettering = next;
  }
}

plettering get_first_lettering()
{
  return first_lettering;
}
