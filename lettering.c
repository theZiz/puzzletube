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
plettering dummy_lettering = NULL;

plettering add_lettering(char* name,int windowx,int windowy)
{
  int globalzoom=spMin(windowx/320,windowy/240);
  plettering lettering = (plettering)malloc(sizeof(tlettering));
  lettering->name = (char*)malloc(strlen(name)+1);
  sprintf(lettering->name,"%s",name);
  char buffer[256];
  if (globalzoom<=1)
    sprintf(buffer,"./images/%s_small.png",name);
  else
    sprintf(buffer,"./images/%s.png",name);
  lettering->surface = spLoadSurface(buffer);
  lettering->next = first_lettering;
  first_lettering = lettering;
  return lettering;
}

void refresh_lettering(int windowx,int windowy)
{
  delete_lettering();
  dummy_lettering = add_lettering("dummy",windowx,windowy);
  add_lettering("1",windowx,windowy);
  add_lettering("2",windowx,windowy);
  add_lettering("3",windowx,windowy);
  add_lettering("go",windowx,windowy);
  add_lettering("chain",windowx,windowy);
  add_lettering("chain_2x",windowx,windowy);
  add_lettering("chain_3x",windowx,windowy);
  add_lettering("chain_4x",windowx,windowy);
  add_lettering("chain_5x",windowx,windowy);
  add_lettering("hyper_chain",windowx,windowy);
  add_lettering("combo",windowx,windowy);
  add_lettering("well",windowx,windowy);
  add_lettering("great",windowx,windowy);
  add_lettering("fantastic",windowx,windowy);
  add_lettering("incredible",windowx,windowy);
  add_lettering("supercalifragilisticexpialidocious",windowx,windowy);
  add_lettering("hurry_up",windowx,windowy);
}

void delete_lettering()
{
  while (first_lettering)
  {
    plettering next = first_lettering->next;
    free(first_lettering->name);
    SDL_FreeSurface(first_lettering->surface);
    free(first_lettering);
    first_lettering = next;
  }
}

SDL_Surface* get_lettering(char* name)
{
  plettering lettering = first_lettering;
  while (lettering != dummy_lettering)
  {
    if (strcmp(lettering->name,name)==0)
      break;
    lettering = lettering->next;
  }
  return lettering->surface;
}
