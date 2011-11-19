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
#include "music.h"
#include "menu.h"
#include "settings.h"
#include "particle.h"
//#include "game.h"
#include "splashscreen.h"

SDL_Surface* screen;
spFontPointer font = NULL;
spFontPointer small_font = NULL;

void resize(Uint16 w,Uint16 h)
{
  //Setup of the new/resized window
  spSelectRenderTarget(spGetWindowSurface());
  spSetPerspective(50.0,(float)spGetWindowSurface()->w/(float)spGetWindowSurface()->h,0.1,100);

  //Font Loading
  if (font)
    spFontDelete(font);
  font = spFontLoad("./font/StayPuft.ttf",18*spGetSizeFactor()>>SP_ACCURACY);
  spFontAddRange(font,' ','~',0);//whole ASCII
  spFontAddBorder(font,65535);
  settings_set_font(font);

  if (small_font)
    spFontDelete(small_font);
  small_font = spFontLoad("./font/StayPuft.ttf",14*spGetSizeFactor()>>SP_ACCURACY);
  spFontAddRange(small_font,' ','~',14823);//whole ASCII
  spFontAddBorder(small_font,48631);
  settings_set_small_font(small_font);
  //Particles
  resize_particle(w,h);
}

int main(int argc, char **argv)
{
  //puzzletube setup
  srand(time(NULL));
  settings_load();
  highscore_load();

  //sparrow3D Init
  spInitCore();
  spInitMath();
  
  //Setup
  screen = spCreateWindow();
  //set_particle_mode(settings_get_alpha_blending());
  resize(screen->w,screen->h);
  
  run_splashscreen(resize);
  init_music();
  init_stars(); //<- Löschen, wenn prepare_game ausauskommentiert.
  //prepare_game_objects(1,9);
  change_music("Midnight Mediation","Nick May");
  highscore_save();
  run_menu(resize);
  //delete_game_objects();
  quit_music();
  spFontDelete(font);
  spFontDelete(small_font);
  spQuitCore();
  return 0;
}
