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
#include "game.h"
#include "splashscreen.h"

#define FONT_LOCATION "./font/Vollkorn-Bold.ttf"
#define FONT_SIZE 16
#define FONT_MIDDLE_SIZE 12
#define FONT_SMALL_SIZE 12
#define FONT_COUNTDOWN_SIZE 128

SDL_Surface* screen;
spFontPointer font = NULL;
spFontPointer small_font = NULL;
spFontPointer middle_font = NULL;
spFontPointer countdown_font = NULL;

void resize(Uint16 w,Uint16 h)
{
  //Setup of the new/resized window
  spSelectRenderTarget(spGetWindowSurface());
  spSetPerspective(50.0,(float)spGetWindowSurface()->w/(float)spGetWindowSurface()->h,0.1,100);

  //Font Loading
  if (font)
    spFontDelete(font);
  font = spFontLoad(FONT_LOCATION,FONT_SIZE*spGetSizeFactor()>>SP_ACCURACY);
  spFontAddRange(font,' ','~',0);//whole ASCII
  //spFontChangeLetter(font,spFontGetLetter(font,'0'),'o',0);
  spFontAddBorder(font,65535);
  spFontMulWidth(font,15<<SP_ACCURACY-4);
  settings_set_font(font);

  if (small_font)
    spFontDelete(small_font);
  small_font = spFontLoad(FONT_LOCATION,FONT_SMALL_SIZE*spGetSizeFactor()>>SP_ACCURACY);
  spFontAddRange(small_font,' ','~',14823);//whole ASCII
  //spFontChangeLetter(small_font,spFontGetLetter(small_font,'0'),'o',14823);
  spFontAddBorder(small_font,48631);
  spFontMulWidth(small_font,15<<SP_ACCURACY-4);
  settings_set_small_font(small_font);

  if (middle_font)
    spFontDelete(middle_font);
  middle_font = spFontLoad(FONT_LOCATION,FONT_MIDDLE_SIZE*spGetSizeFactor()>>SP_ACCURACY);
  spFontAddRange(middle_font,' ','~',0);//whole ASCII
  //spFontChangeLetter(middle_font,spFontGetLetter(middle_font,'0'),'o',0);
  spFontAddBorder(middle_font,65535);
  spFontMulWidth(middle_font,15<<SP_ACCURACY-4);
  settings_set_middle_font(middle_font);

  if (countdown_font)
    spFontDelete(countdown_font);
  countdown_font = spFontLoad(FONT_LOCATION,FONT_COUNTDOWN_SIZE*spGetSizeFactor()>>SP_ACCURACY);
  spFontAddRange(countdown_font,' ','~',0);//whole ASCII
  spFontAddBorder(countdown_font,65535);
  settings_set_countdown_font(countdown_font);

  //Particles
  resize_particle(w,h);
  init_stars();  
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
  prepare_game_objects(1);
  change_music("Midnight Mediation","Nick May");
  highscore_save();
  run_menu(resize);
  delete_game_objects();
  quit_music();
  spFontDelete(font);
  spFontDelete(small_font);
  spFontDelete(middle_font);
  spQuitCore();
  return 0;
}
