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
#include "menu.h"
#include "game.h"

#define MENUSIZE 1200

int menu_counter = 0;
int state = 0;
int nextstate = 0;
Sint32 menu_choice;
int menu_move = 0;
int menu_fade;
int menu_wait = 0;

void draw_menu(void)
{
  Sint32* modellViewMatrix=engineGetModellViewMatrix();
  plight light=engineGetLightPointer();
  int engineWindowX=engineGetWindowX();
  int engineWindowY=engineGetWindowY();
  engineClearScreen(0);
  setModellViewMatrixIdentity();

  engineTranslate(0,0,-20<<ACCURACY);
  engineRotate(0,1<<ACCURACY,0,menu_counter*10);

  draw_stars();
  
  setModellViewMatrixIdentity();
  engineTranslate(0,0,-20<<ACCURACY);
  engineTranslate(menu_fade<<ACCURACY-6,0,0);
  
  switch (state)
  {
    case 0: //menu
      engineDrawTextMXMY(0,( 10<<ACCURACY-1)+mysin(menu_counter*300+6*MY_PI*2/7)/4,0,"PUZZLE TUBE");
      engineDrawTextMXMY(0,(  5<<ACCURACY-1)+mysin(menu_counter*300+5*MY_PI*2/7)/4,0,"Arcade");
      engineDrawTextMXMY(0,(  1<<ACCURACY-1)+mysin(menu_counter*300+4*MY_PI*2/7)/4,0,"Free Game");
      engineDrawTextMXMY(0,(- 3<<ACCURACY-1)+mysin(menu_counter*300+3*MY_PI*2/7)/4,0,"Highscore");
      engineDrawTextMXMY(0,(- 7<<ACCURACY-1)+mysin(menu_counter*300+2*MY_PI*2/7)/4,0,"Settings");
      engineDrawTextMXMY(0,(-11<<ACCURACY-1)+mysin(menu_counter*300+1*MY_PI*2/7)/4,0,"About");
      engineDrawTextMXMY(0,(-15<<ACCURACY-1)+mysin(menu_counter*300+0*MY_PI*2/7)/4,0,"Quit");
      //Left circle
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+5*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+5*MY_PI/10)-menu_choice*4/2,0,getBigParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+3*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+3*MY_PI/10)-menu_choice*4/2,0,getMiddleParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+2*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+2*MY_PI/10)-menu_choice*4/2,0,getMiddleParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+1*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+1*MY_PI/10)-menu_choice*4/2,0,getSmallParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+0*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+0*MY_PI/10)-menu_choice*4/2,0,getSmallParticle());
      
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+15*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+15*MY_PI/10)-menu_choice*4/2,0,getBigParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+13*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+13*MY_PI/10)-menu_choice*4/2,0,getMiddleParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+12*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+12*MY_PI/10)-menu_choice*4/2,0,getMiddleParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+11*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+11*MY_PI/10)-menu_choice*4/2,0,getSmallParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+10*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+10*MY_PI/10)-menu_choice*4/2,0,getSmallParticle());
      //Right circle
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+5*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+5*MY_PI/10)-menu_choice*4/2,0,getBigParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+3*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+3*MY_PI/10)-menu_choice*4/2,0,getMiddleParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+2*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+2*MY_PI/10)-menu_choice*4/2,0,getMiddleParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+1*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+1*MY_PI/10)-menu_choice*4/2,0,getSmallParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+0*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+0*MY_PI/10)-menu_choice*4/2,0,getSmallParticle());

      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+15*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+15*MY_PI/10)-menu_choice*4/2,0,getBigParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+13*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+13*MY_PI/10)-menu_choice*4/2,0,getMiddleParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+12*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+12*MY_PI/10)-menu_choice*4/2,0,getMiddleParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+11*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+11*MY_PI/10)-menu_choice*4/2,0,getSmallParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+10*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+10*MY_PI/10)-menu_choice*4/2,0,getSmallParticle());
      break;
    case 1: //options
      engineDrawTextMXMY(0,( 10<<ACCURACY-1)+mysin(menu_counter*300+7*MY_PI/4)/4,0,"SETTINGS");
      switch (settings_get_stone_quality())
      {
        case 0: engineDrawTextMXMY(0,(  5<<ACCURACY-1)+mysin(menu_counter*300+6*MY_PI/4)/4,0,"Stone Quality: Flat"); break;
        case 1: engineDrawTextMXMY(0,(  5<<ACCURACY-1)+mysin(menu_counter*300+6*MY_PI/4)/4,0,"Stone Quality: Okay"); break;
        case 2: engineDrawTextMXMY(0,(  5<<ACCURACY-1)+mysin(menu_counter*300+6*MY_PI/4)/4,0,"Stone Quality: Perfect"); break;
      }
      switch (settings_get_stars_rotating())
      {
        case 0: engineDrawTextMXMY(0,(  2<<ACCURACY-1)+mysin(menu_counter*300+5*MY_PI/4)/4,0,"Stars: Off"); break;
        case 1: engineDrawTextMXMY(0,(  2<<ACCURACY-1)+mysin(menu_counter*300+5*MY_PI/4)/4,0,"Stars: Still"); break;
        case 2: engineDrawTextMXMY(0,(  2<<ACCURACY-1)+mysin(menu_counter*300+5*MY_PI/4)/4,0,"Stars: Rotating"); break;
      }
      if (settings_get_particles())
        engineDrawTextMXMY(0,(- 1<<ACCURACY-1)+mysin(menu_counter*300+4*MY_PI/4)/4,0,"Particles: On");
      else
        engineDrawTextMXMY(0,(- 1<<ACCURACY-1)+mysin(menu_counter*300+4*MY_PI/4)/4,0,"Particles: Off");

      if (settings_get_alpha_blending())
        engineDrawTextMXMY(0,(- 4<<ACCURACY-1)+mysin(menu_counter*300+3*MY_PI/4)/4,0,"Blending: Smooth");
      else
        engineDrawTextMXMY(0,(- 4<<ACCURACY-1)+mysin(menu_counter*300+3*MY_PI/4)/4,0,"Blending: Hard");

      if (settings_get_font_quality())
        engineDrawTextMXMY(0,(- 7<<ACCURACY-1)+mysin(menu_counter*300+2*MY_PI/4)/4,0,"Font Quality: Good");
      else
        engineDrawTextMXMY(0,(- 7<<ACCURACY-1)+mysin(menu_counter*300+2*MY_PI/4)/4,0,"Font Quality: Fast");

      char buffer[256];
      sprintf(buffer,"Volume %i%%",settings_get_volume());
      engineDrawTextMXMY(0,(-11<<ACCURACY-1)+mysin(menu_counter*300+1*MY_PI/4)/4,0,buffer);
      engineDrawTextMXMY(0,(-14<<ACCURACY-1)+mysin(menu_counter*300+0*MY_PI/4)/4,0,"Back to Menu");
      //Left circle
      engineDrawSurface((-7<<ACCURACY)+mycos(menu_counter*700+5*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+5*MY_PI/10)-menu_choice*3/2,0,getBigParticle());
      engineDrawSurface((-7<<ACCURACY)+mycos(menu_counter*700+3*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+3*MY_PI/10)-menu_choice*3/2,0,getMiddleParticle());
      engineDrawSurface((-7<<ACCURACY)+mycos(menu_counter*700+2*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+2*MY_PI/10)-menu_choice*3/2,0,getMiddleParticle());
      engineDrawSurface((-7<<ACCURACY)+mycos(menu_counter*700+1*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+1*MY_PI/10)-menu_choice*3/2,0,getSmallParticle());
      engineDrawSurface((-7<<ACCURACY)+mycos(menu_counter*700+0*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+0*MY_PI/10)-menu_choice*3/2,0,getSmallParticle());
      
      engineDrawSurface((-7<<ACCURACY)+mycos(menu_counter*700+15*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+15*MY_PI/10)-menu_choice*3/2,0,getBigParticle());
      engineDrawSurface((-7<<ACCURACY)+mycos(menu_counter*700+13*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+13*MY_PI/10)-menu_choice*3/2,0,getMiddleParticle());
      engineDrawSurface((-7<<ACCURACY)+mycos(menu_counter*700+12*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+12*MY_PI/10)-menu_choice*3/2,0,getMiddleParticle());
      engineDrawSurface((-7<<ACCURACY)+mycos(menu_counter*700+11*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+11*MY_PI/10)-menu_choice*3/2,0,getSmallParticle());
      engineDrawSurface((-7<<ACCURACY)+mycos(menu_counter*700+10*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+10*MY_PI/10)-menu_choice*3/2,0,getSmallParticle());
      //Right circle
      engineDrawSurface(( 7<<ACCURACY)-mycos(menu_counter*700+5*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+5*MY_PI/10)-menu_choice*3/2,0,getBigParticle());
      engineDrawSurface(( 7<<ACCURACY)-mycos(menu_counter*700+3*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+3*MY_PI/10)-menu_choice*3/2,0,getMiddleParticle());
      engineDrawSurface(( 7<<ACCURACY)-mycos(menu_counter*700+2*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+2*MY_PI/10)-menu_choice*3/2,0,getMiddleParticle());
      engineDrawSurface(( 7<<ACCURACY)-mycos(menu_counter*700+1*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+1*MY_PI/10)-menu_choice*3/2,0,getSmallParticle());
      engineDrawSurface(( 7<<ACCURACY)-mycos(menu_counter*700+0*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+0*MY_PI/10)-menu_choice*3/2,0,getSmallParticle());

      engineDrawSurface(( 7<<ACCURACY)-mycos(menu_counter*700+15*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+15*MY_PI/10)-menu_choice*3/2,0,getBigParticle());
      engineDrawSurface(( 7<<ACCURACY)-mycos(menu_counter*700+13*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+13*MY_PI/10)-menu_choice*3/2,0,getMiddleParticle());
      engineDrawSurface(( 7<<ACCURACY)-mycos(menu_counter*700+12*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+12*MY_PI/10)-menu_choice*3/2,0,getMiddleParticle());
      engineDrawSurface(( 7<<ACCURACY)-mycos(menu_counter*700+11*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+11*MY_PI/10)-menu_choice*3/2,0,getSmallParticle());
      engineDrawSurface(( 7<<ACCURACY)-mycos(menu_counter*700+10*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+10*MY_PI/10)-menu_choice*3/2,0,getSmallParticle());
      break;
    case 2: //Free Game
      engineDrawTextMXMY(0,( 10<<ACCURACY-1)+mysin(menu_counter*300+5*MY_PI/3)/4,0,"FREE GAME");
      engineDrawTextMXMY(0,(  5<<ACCURACY-1)+mysin(menu_counter*300+1*MY_PI/3)/4,0,"<<< Play >>>");
      if (settings_get_mode() == 0)
        engineDrawTextMXMY(0,(  0<<ACCURACY-1)+mysin(menu_counter*300+4*MY_PI/3)/4,0,"Game Mode: Points");
      else
        engineDrawTextMXMY(0,(  0<<ACCURACY-1)+mysin(menu_counter*300+4*MY_PI/3)/4,0,"Game Mode: Time Stole");
      sprintf(buffer,"Different stone kinds: %i",settings_get_color());
      engineDrawTextMXMY(0,(- 5<<ACCURACY-1)+mysin(menu_counter*300+3*MY_PI/3)/4,0,buffer);
      sprintf(buffer,"Difficulty: %i",settings_get_difficult());
      engineDrawTextMXMY(0,(-10<<ACCURACY-1)+mysin(menu_counter*300+2*MY_PI/3)/4,0,buffer);
      engineDrawTextMXMY(0,(-15<<ACCURACY-1)+mysin(menu_counter*300+0*MY_PI/3)/4,0,"Back to Menu");
      //Left circle
      engineDrawSurface((-8<<ACCURACY)+mycos(menu_counter*700+5*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+5*MY_PI/10)-menu_choice*5/2,0,getBigParticle());
      engineDrawSurface((-8<<ACCURACY)+mycos(menu_counter*700+3*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+3*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface((-8<<ACCURACY)+mycos(menu_counter*700+2*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+2*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface((-8<<ACCURACY)+mycos(menu_counter*700+1*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+1*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      engineDrawSurface((-8<<ACCURACY)+mycos(menu_counter*700+0*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+0*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      
      engineDrawSurface((-8<<ACCURACY)+mycos(menu_counter*700+15*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+15*MY_PI/10)-menu_choice*5/2,0,getBigParticle());
      engineDrawSurface((-8<<ACCURACY)+mycos(menu_counter*700+13*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+13*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface((-8<<ACCURACY)+mycos(menu_counter*700+12*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+12*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface((-8<<ACCURACY)+mycos(menu_counter*700+11*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+11*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      engineDrawSurface((-8<<ACCURACY)+mycos(menu_counter*700+10*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+10*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      //Right circle
      engineDrawSurface(( 8<<ACCURACY)-mycos(menu_counter*700+5*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+5*MY_PI/10)-menu_choice*5/2,0,getBigParticle());
      engineDrawSurface(( 8<<ACCURACY)-mycos(menu_counter*700+3*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+3*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface(( 8<<ACCURACY)-mycos(menu_counter*700+2*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+2*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface(( 8<<ACCURACY)-mycos(menu_counter*700+1*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+1*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      engineDrawSurface(( 8<<ACCURACY)-mycos(menu_counter*700+0*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+0*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());

      engineDrawSurface(( 8<<ACCURACY)-mycos(menu_counter*700+15*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+15*MY_PI/10)-menu_choice*5/2,0,getBigParticle());
      engineDrawSurface(( 8<<ACCURACY)-mycos(menu_counter*700+13*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+13*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface(( 8<<ACCURACY)-mycos(menu_counter*700+12*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+12*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface(( 8<<ACCURACY)-mycos(menu_counter*700+11*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+11*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      engineDrawSurface(( 8<<ACCURACY)-mycos(menu_counter*700+10*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+10*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      break;
    case 3: //High Score
      engineDrawTextMXMY(0,( 10<<ACCURACY-1)+mysin(menu_counter*300+6*MY_PI*2/7)/4,0,"HIGHSCORE");
      engineDrawTextMXMY(0,(  5<<ACCURACY-1)+mysin(menu_counter*300+5*MY_PI*2/7)/4,0,"Zaz - 1.000.000");
      engineDrawTextMXMY(0,(  2<<ACCURACY-1)+mysin(menu_counter*300+4*MY_PI*2/7)/4,0,"Zez - 500.000");
      engineDrawTextMXMY(0,(- 1<<ACCURACY-1)+mysin(menu_counter*300+3*MY_PI*2/7)/4,0,"Ziz - 250.000");
      engineDrawTextMXMY(0,(- 4<<ACCURACY-1)+mysin(menu_counter*300+2*MY_PI*2/7)/4,0,"Zoz - 125.000");
      engineDrawTextMXMY(0,(- 7<<ACCURACY-1)+mysin(menu_counter*300+1*MY_PI*2/7)/4,0,"Zuz - 62.500");
      engineDrawTextMXMY(0,(-10<<ACCURACY-1)+mysin(menu_counter*300+0*MY_PI*2/7)/4,0,"Zyz - 62.500");
      engineDrawTextMXMY(0,(-15<<ACCURACY-1)+mysin(menu_counter*300+-1*MY_PI*2/7)/4,0,"Time Stole - Difficulty 0");
      engineDrawTextMXMY(0,(-18<<ACCURACY-1)+mysin(menu_counter*300+-2*MY_PI*2/7)/4,0,"4 different stones");

      //Left circle
      engineDrawSurface((-9<<ACCURACY)+mycos(menu_counter*700+5*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+5*MY_PI/10)-(21<<ACCURACY-1),0,getBigParticle());
      engineDrawSurface((-9<<ACCURACY)+mycos(menu_counter*700+3*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+3*MY_PI/10)-(21<<ACCURACY-1),0,getMiddleParticle());
      engineDrawSurface((-9<<ACCURACY)+mycos(menu_counter*700+2*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+2*MY_PI/10)-(21<<ACCURACY-1),0,getMiddleParticle());
      engineDrawSurface((-9<<ACCURACY)+mycos(menu_counter*700+1*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+1*MY_PI/10)-(21<<ACCURACY-1),0,getSmallParticle());
      engineDrawSurface((-9<<ACCURACY)+mycos(menu_counter*700+0*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+0*MY_PI/10)-(21<<ACCURACY-1),0,getSmallParticle());
      
      engineDrawSurface((-9<<ACCURACY)+mycos(menu_counter*700+15*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+15*MY_PI/10)-(21<<ACCURACY-1),0,getBigParticle());
      engineDrawSurface((-9<<ACCURACY)+mycos(menu_counter*700+13*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+13*MY_PI/10)-(21<<ACCURACY-1),0,getMiddleParticle());
      engineDrawSurface((-9<<ACCURACY)+mycos(menu_counter*700+12*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+12*MY_PI/10)-(21<<ACCURACY-1),0,getMiddleParticle());
      engineDrawSurface((-9<<ACCURACY)+mycos(menu_counter*700+11*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+11*MY_PI/10)-(21<<ACCURACY-1),0,getSmallParticle());
      engineDrawSurface((-9<<ACCURACY)+mycos(menu_counter*700+10*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+10*MY_PI/10)-(21<<ACCURACY-1),0,getSmallParticle());
      
      engineDrawTextMXMY(-9<<ACCURACY,-33<<ACCURACY-2,0,"L");
      
      //Right circle
      engineDrawSurface((9<<ACCURACY)-mycos(menu_counter*700+5*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+5*MY_PI/10)-(21<<ACCURACY-1),0,getBigParticle());
      engineDrawSurface((9<<ACCURACY)-mycos(menu_counter*700+3*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+3*MY_PI/10)-(21<<ACCURACY-1),0,getMiddleParticle());
      engineDrawSurface((9<<ACCURACY)-mycos(menu_counter*700+2*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+2*MY_PI/10)-(21<<ACCURACY-1),0,getMiddleParticle());
      engineDrawSurface((9<<ACCURACY)-mycos(menu_counter*700+1*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+1*MY_PI/10)-(21<<ACCURACY-1),0,getSmallParticle());
      engineDrawSurface((9<<ACCURACY)-mycos(menu_counter*700+0*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+0*MY_PI/10)-(21<<ACCURACY-1),0,getSmallParticle());

      engineDrawSurface((9<<ACCURACY)-mycos(menu_counter*700+15*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+15*MY_PI/10)-(21<<ACCURACY-1),0,getBigParticle());
      engineDrawSurface((9<<ACCURACY)-mycos(menu_counter*700+13*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+13*MY_PI/10)-(21<<ACCURACY-1),0,getMiddleParticle());
      engineDrawSurface((9<<ACCURACY)-mycos(menu_counter*700+12*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+12*MY_PI/10)-(21<<ACCURACY-1),0,getMiddleParticle());
      engineDrawSurface((9<<ACCURACY)-mycos(menu_counter*700+11*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+11*MY_PI/10)-(21<<ACCURACY-1),0,getSmallParticle());
      engineDrawSurface((9<<ACCURACY)-mycos(menu_counter*700+10*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+10*MY_PI/10)-(21<<ACCURACY-1),0,getSmallParticle());

      engineDrawTextMXMY(9<<ACCURACY,-33<<ACCURACY-2,0,"R");
      break;

    case 4: //About
      engineDrawTextMXMY(0,( 10<<ACCURACY-1)+mysin(menu_counter*300+6*MY_PI*2/7)/4,0,"ABOUT");
      engineDrawTextMXMY(0,(  5<<ACCURACY-1)+mysin(menu_counter*300+5*MY_PI*2/7)/4,0,"Code Monkey: Ziz");
      engineDrawTextMXMY(0,(  1<<ACCURACY-1)+mysin(menu_counter*300+4*MY_PI*2/7)/4,0,"Sound: Nick May");
      engineDrawTextMXMY(0,(- 3<<ACCURACY-1)+mysin(menu_counter*300+3*MY_PI*2/7)/4,0,"Backers of the RIOTdigital Contest:");
      engineDrawTextMXMY(0,(- 6<<ACCURACY-1)+mysin(menu_counter*300+2*MY_PI*2/7)/4,0,"Pandora");
      engineDrawTextMXMY(0,(- 9<<ACCURACY-1)+mysin(menu_counter*300+1*MY_PI*2/7)/4,0,"GPH");
      engineDrawTextMXMY(0,(-12<<ACCURACY-1)+mysin(menu_counter*300+0*MY_PI*2/7)/4,0,"DINGOO DIGITAL USA");
      engineDrawTextMXMY(0,(-15<<ACCURACY-1)+mysin(menu_counter*300+-1*MY_PI*2/7)/4,0,"Etronixmart");
      break;
  }

  engineDrawList();
  draw_music();
  engineFlip();  
}

int calc_menu(Uint32 steps)
{
  menu_counter += steps;
  calc_music(steps);
  pEngineInput engineInput = engineGetInput();
  if (menu_fade>0)
  {
    menu_fade-=steps*3;
    if (menu_fade<=0) {
      menu_fade = 0;
      rotating_sound_off();
    }
    return 0;
  }
  if (menu_fade<0)
  {
    menu_fade-=steps*3;
    if (menu_fade<=-MENUSIZE)
    {
      menu_fade = MENUSIZE;
      if (nextstate>=0)
      {
        state = nextstate;
        menu_choice = 0;
      }
      else
      switch (nextstate)
      {
        case -2: //Free Game
          prepare_game_objects(0,settings_get_color());
          printf("%i\n",menu_counter);
          menu_counter = run_game(1,settings_get_mode(),settings_get_difficult(),menu_counter*10)/10;
          printf("%i\n",menu_counter);
          engineInput->button[BUTTON_START] = 0;
          engineInput->button[BUTTON_A] = 0;
          engineInput->button[BUTTON_B] = 0;
          engineInput->button[BUTTON_X] = 0;
          engineInput->button[BUTTON_Y] = 0;
          state = 0;
          rotating_sound_off();
          break;
        case -4:
          rotating_sound_off();
          return 1;
      }
      return 0;
    }
  }

  int i;
  
  switch (state)
  {
  case 0: //menu
    if (engineGetAxis(1)<0 && menu_move==0 && (menu_choice>>ACCURACY)<5)
      menu_move = 1;
    if (engineGetAxis(1)>0 && menu_move==0 && (menu_choice>>ACCURACY)>0)
      menu_move = -1;
    
    if (menu_move != 0)
    {
      move_sound_on();
      for (i=0;i<steps;i++)
      {
        menu_choice += menu_move*4 << ACCURACY-10;
        if (menu_choice == ((menu_choice >>ACCURACY)<<ACCURACY))
          menu_move = 0;
      }
    }
    else
      move_sound_off();
    if (menu_move == 0 && (engineInput->button[BUTTON_START] ||
        engineInput->button[BUTTON_A] || engineInput->button[BUTTON_B] ||
        engineInput->button[BUTTON_X] || engineInput->button[BUTTON_Y]))
    {
      switch (menu_choice>>ACCURACY)
      {
        case 1: // Free Game
          nextstate = 2;
          menu_fade = -1;
          rotating_sound_on();
          break;
        case 2: // Highscore
          nextstate = 3;
          menu_fade = -1;
          rotating_sound_on();
          break;
        case 3: // Options
          nextstate = 1;
          menu_fade = -1;
          rotating_sound_on();
          break;
        case 4: // About
          nextstate = 4;
          menu_fade = -1;
          rotating_sound_on();
          break;
        case 5: // Quit
          nextstate = -4;
          menu_fade = -1;
          rotating_sound_on();
          break;
      }
    }
    break;
  case 1: //options
    if (engineGetAxis(1)<0 && menu_move==0 && (menu_choice>>ACCURACY)<6)
      menu_move = 1;
    if (engineGetAxis(1)>0 && menu_move==0 && (menu_choice>>ACCURACY)>0)
      menu_move = -1;
    
    if (menu_move != 0)
    {
      move_sound_on();
      for (i=0;i<steps;i++)
      {
        menu_choice += menu_move*4 << ACCURACY-10;
        if (menu_choice == ((menu_choice >>ACCURACY)<<ACCURACY))
          menu_move = 0;
      }
    }
    else
      move_sound_off();
    for (i = 0;i < steps;i++)
    {
      menu_wait--;
      if (menu_move == 0 && (menu_choice>>ACCURACY) == 5 && engineGetAxis(0)<0 && menu_wait <= 0 && settings_get_volume()>0)
      {
        settings_set_volume(settings_get_volume()-1);
        set_volume(settings_get_volume());
        menu_wait = 25;
      }
      if (menu_move == 0 && (menu_choice>>ACCURACY) == 5 && engineGetAxis(0)>0 && menu_wait <= 0 && settings_get_volume()<100)
      {
        settings_set_volume(settings_get_volume()+1);
        set_volume(settings_get_volume());
        menu_wait = 25;
      }
    }
    if (menu_move == 0 && (menu_choice>>ACCURACY) == 0 && engineGetAxis(0)<0 && menu_wait <= 0 && settings_get_stone_quality()>0)
    {
      settings_set_stone_quality(settings_get_stone_quality()-1);
      engineSetAxis(0,0);
    }
    if (menu_move == 0 && (menu_choice>>ACCURACY) == 0 && engineGetAxis(0)>0 && menu_wait <= 0 && settings_get_stone_quality()<2)
    {
      settings_set_stone_quality(settings_get_stone_quality()+1);
      engineSetAxis(0,0);
    }
    if (menu_move == 0 && (menu_choice>>ACCURACY) == 1 && engineGetAxis(0)<0 && menu_wait <= 0 && settings_get_stars_rotating()>0)
    {
      settings_set_stars_rotating(settings_get_stars_rotating()-1);
      engineSetAxis(0,0);
    }
    if (menu_move == 0 && (menu_choice>>ACCURACY) == 1 && engineGetAxis(0)>0 && menu_wait <= 0 && settings_get_stars_rotating()<2)
    {
      settings_set_stars_rotating(settings_get_stars_rotating()+1);
      engineSetAxis(0,0);
    }
    if (menu_move == 0 && (engineInput->button[BUTTON_START] ||
        engineInput->button[BUTTON_A] || engineInput->button[BUTTON_B] ||
        engineInput->button[BUTTON_X] || engineInput->button[BUTTON_Y]))
    {
      engineInput->button[BUTTON_START] = 0;
      engineInput->button[BUTTON_A] = 0;
      engineInput->button[BUTTON_B] = 0;
      engineInput->button[BUTTON_X] = 0;
      engineInput->button[BUTTON_Y] = 0;
      switch (menu_choice>>ACCURACY)
      {
        case 0: //Stone Quality
          settings_set_stone_quality((settings_get_stone_quality()+1)%3);
          break;
        case 1: //Stars Rotating
          settings_set_stars_rotating((settings_get_stars_rotating()+1)%3);
          break;
        case 2: //Particles
          settings_set_particles((settings_get_particles()+1)%2);
          break;
        case 3: //Alpha Blending
          settings_set_alpha_blending((settings_get_alpha_blending()+1)%2);
          set_particle_mode(settings_get_alpha_blending());
          resize_particle(engineGetWindowX(),engineGetWindowY());
          break;
        case 4: //Font Quality
          settings_set_font_quality((settings_get_font_quality()+1)%2);
          set_font_quality(settings_get_font_quality());
          loadKeyMap();
          break;
        case 6: //Back
          settings_save();
          nextstate = 0;
          menu_fade = -1;
          rotating_sound_on();
          break;
      }
    }
    break;
  case 2: //free game
    if (engineGetAxis(1)<0 && menu_move==0 && (menu_choice>>ACCURACY)<4)
      menu_move = 1;
    if (engineGetAxis(1)>0 && menu_move==0 && (menu_choice>>ACCURACY)>0)
      menu_move = -1;
    
    if (menu_move != 0)
    {
      move_sound_on();
      for (i=0;i<steps;i++)
      {
        menu_choice += menu_move*4 << ACCURACY-10;
        if (menu_choice == ((menu_choice >>ACCURACY)<<ACCURACY))
          menu_move = 0;
      }
    }
    else
      move_sound_off();
    if (menu_move == 0 && (menu_choice>>ACCURACY) == 2 && engineGetAxis(0)<0 && menu_wait <= 0 && settings_get_color()>4)
    {
      settings_set_color(settings_get_color()-1);
      engineSetAxis(0,0);
    }
    if (menu_move == 0 && (menu_choice>>ACCURACY) == 2 && engineGetAxis(0)>0 && menu_wait <= 0 && settings_get_color()<9)
    {
      settings_set_color(settings_get_color()+1);
      engineSetAxis(0,0);
    }
    if (menu_move == 0 && (menu_choice>>ACCURACY) == 3 && engineGetAxis(0)<0 && menu_wait <= 0 && settings_get_difficult()>0)
    {
      settings_set_difficult(settings_get_difficult()-1);
      engineSetAxis(0,0);
    }
    if (menu_move == 0 && (menu_choice>>ACCURACY) == 3 && engineGetAxis(0)>0 && menu_wait <= 0 && settings_get_difficult()<9)
    {
      settings_set_difficult(settings_get_difficult()+1);
      engineSetAxis(0,0);
    }
    if (menu_move == 0 && (engineInput->button[BUTTON_START] ||
        engineInput->button[BUTTON_A] || engineInput->button[BUTTON_B] ||
        engineInput->button[BUTTON_X] || engineInput->button[BUTTON_Y]))
    {
      engineInput->button[BUTTON_START] = 0;
      engineInput->button[BUTTON_A] = 0;
      engineInput->button[BUTTON_B] = 0;
      engineInput->button[BUTTON_X] = 0;
      engineInput->button[BUTTON_Y] = 0;
      switch (menu_choice>>ACCURACY)
      {
        case 0: //Play
          settings_save();
          nextstate = -2;
          menu_fade = -1;
          rotating_sound_on();
          break;
        case 1: //Game Mode
          settings_set_mode((settings_get_mode()+1)%2);
          break;
        case 2: //colors
          settings_set_color((settings_get_color()-3)%6+4);
          break;
        case 3: //difficulty
          settings_set_difficult((settings_get_difficult()+1)%10);
          break;
        case 4: //Back
          nextstate = 0;
          menu_fade = -1;
          rotating_sound_on();
          break;
      }
    }
    break;
  case 3: case 4: //high score, about
    if (menu_move == 0 && (engineInput->button[BUTTON_START] ||
        engineInput->button[BUTTON_A] || engineInput->button[BUTTON_B] ||
        engineInput->button[BUTTON_X] || engineInput->button[BUTTON_Y]))
    {
      engineInput->button[BUTTON_START] = 0;
      engineInput->button[BUTTON_A] = 0;
      engineInput->button[BUTTON_B] = 0;
      engineInput->button[BUTTON_X] = 0;
      engineInput->button[BUTTON_Y] = 0;
      nextstate = 0;
      menu_fade = -1;
      rotating_sound_on();
    }
    break;
  }
  return 0;
}

void run_menu()
{
  menu_fade = MENUSIZE;
  menu_choice = 0;
  rotating_sound_on();
  engineLoop(draw_menu,calc_menu,10);
}

