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

int menu_counter = 0;
int state = 0;
Sint32 menu_choice = 0;
int menu_move = 0;

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
  
  switch (state)
  {
    case 0: //menu
      engineDrawTextMXMY(0,( 10<<ACCURACY-1)+mysin(menu_counter*300+5*MY_PI/3)/4,0,"PUZZLE TUBE");
      engineDrawTextMXMY(0,(  5<<ACCURACY-1)+mysin(menu_counter*300+4*MY_PI/3)/4,0,"Arcade");
      engineDrawTextMXMY(0,(- 0<<ACCURACY-1)+mysin(menu_counter*300+3*MY_PI/3)/4,0,"Tutorial");
      engineDrawTextMXMY(0,(- 5<<ACCURACY-1)+mysin(menu_counter*300+2*MY_PI/3)/4,0,"Free Game");
      engineDrawTextMXMY(0,(-10<<ACCURACY-1)+mysin(menu_counter*300+1*MY_PI/3)/4,0,"Settings");
      engineDrawTextMXMY(0,(-15<<ACCURACY-1)+mysin(menu_counter*300+0*MY_PI/3)/4,0,"Quit");
      setModellViewMatrixIdentity();
      engineTranslate(0,0,-20<<ACCURACY);
      //Left circle
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+5*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+5*MY_PI/10)-menu_choice*5/2,0,getBigParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+3*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+3*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+2*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+2*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+1*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+1*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+0*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+0*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+15*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+15*MY_PI/10)-menu_choice*5/2,0,getBigParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+13*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+13*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+12*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+12*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+11*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+11*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      engineDrawSurface((-4<<ACCURACY)+mycos(menu_counter*700+10*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+10*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      //Right circle
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+5*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+5*MY_PI/10)-menu_choice*5/2,0,getBigParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+3*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+3*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+2*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+2*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+1*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+1*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+0*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+0*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());

      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+15*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+15*MY_PI/10)-menu_choice*5/2,0,getBigParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+13*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+13*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+12*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+12*MY_PI/10)-menu_choice*5/2,0,getMiddleParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+11*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+11*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      engineDrawSurface(( 4<<ACCURACY)-mycos(menu_counter*700+10*MY_PI/10),( 3<<ACCURACY)+mysin(menu_counter*700+10*MY_PI/10)-menu_choice*5/2,0,getSmallParticle());
      break;
    case 1: //options
      break;
    
  }

  engineDrawList();
  draw_music();
  engineFlip();  
}

int calc_menu(Uint32 steps)
{
  int i;
  menu_counter += steps;
  calc_music(steps);
  pEngineInput engineInput = engineGetInput();
  
  switch (state)
  {
  case 0: //menu
    if (engineInput->axis[1]<0 && menu_move==0 && (menu_choice>>ACCURACY)<4)
      menu_move = 1;
    if (engineInput->axis[1]>0 && menu_move==0 && (menu_choice>>ACCURACY)>0)
      menu_move = -1;
    
    if (menu_move != 0)
    {
      for (i=0;i<steps;i++)
      {
        menu_choice += menu_move*4 << ACCURACY-10;
        if (menu_choice == ((menu_choice >>ACCURACY)<<ACCURACY))
          menu_move = 0;
      }
    }
    if (menu_move == 0 && (engineInput->button[BUTTON_START] ||
        engineInput->button[BUTTON_A] || engineInput->button[BUTTON_B] ||
        engineInput->button[BUTTON_X] || engineInput->button[BUTTON_Y]))
    {
      switch (menu_choice>>ACCURACY)
      {
        case 2: state = 2; // Free Game
        case 4: return 1;
      }
    }
    break;
  }
  return 0;
}

void run_menu()
{
  engineLoop(draw_menu,calc_menu,10);
}

