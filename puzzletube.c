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
#include "music.h"
#include "game.h"
#include "splashscreen.h"

int main(int argc, char **argv)
{
  srand(time(NULL));
  engineSetKeymap("./font/StayPuft.ttf");
  settings_load();
  highscore_load();
  set_font_quality(settings_get_font_quality());
  set_particle_mode(settings_get_alpha_blending());
  initEngine();
  run_splashscreen();
  init_music();
  refresh_lettering(engineGetWindowX(),engineGetWindowY());
  prepare_game_objects(1,9);
  change_music("Midnight Mediation","Nick May");
  run_menu();
  delete_lettering();
  delete_game_objects();
  quit_music();
  quitEngine();
  return 0;
}
