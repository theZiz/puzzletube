/*
 The contents of this file are subject to the Mozilla Public License        
 Version 1.1 (the "License",font); you may not use this file except in           
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
#include "menu.h"
#include "game.h"
#include "settings.h"
#include "particle.h"

#define MENUSIZE 1200

void (*menu_resize)(Uint16 w,Uint16 h);

int menu_counter = 0;
int state = 0;
int nextstate = 0;
Sint32 menu_choice;
int menu_move = 0;
int menu_fade;
int menu_wait = 0;
int menu_block = 0;

void draw_menu(void)
{
  #ifdef MENU_DEBUG
    printf("%i:   Start drawing menu\n",SDL_GetTicks());
  #endif
  Sint32* modellViewMatrix=spGetMatrix();
  int engineWindowX=spGetWindowSurface()->w;
  int engineWindowY=spGetWindowSurface()->h;
  spFontPointer font = settings_get_font();
  spFontPointer small_font = settings_get_small_font();
  
  spSetAlphaTest(1);
  spSetZSet(0);
  spSetZTest(0);
  spResetZBuffer();
  //spClearTarget(BACKGROUND_COLOR);
  
  spIdentity();
  spTranslate(0,0,-20<<SP_ACCURACY);
  draw_stars(menu_counter*10);
  spRotate(0,1<<SP_ACCURACY,0,menu_counter*10);
  #ifdef MENU_DEBUG
    printf("%i:   Drew stars\n",SDL_GetTicks());
  #endif
  
  spIdentity();
  draw_music();
  #ifdef MENU_DEBUG
    printf("%i:   Drew music\n",SDL_GetTicks());
  #endif
  spTranslate(0,0,-20<<SP_ACCURACY);
  spTranslate(menu_fade<<SP_ACCURACY-6,0,0);  
  int mode;
  int colors;
  int difficult;
  int i;
  switch (state)
  {
    case 0: //menu
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),1*engineWindowY/7+(spSin(menu_counter*300+6*SP_PI*2/7)>>SP_ACCURACY-2),-1,"PUZZLE TUBE",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),2*engineWindowY/7+(spSin(menu_counter*300+4*SP_PI*2/7)>>SP_ACCURACY-2),-1,"Free Game",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),3*engineWindowY/7+(spSin(menu_counter*300+3*SP_PI*2/7)>>SP_ACCURACY-2),-1,"Highscore",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),4*engineWindowY/7+(spSin(menu_counter*300+2*SP_PI*2/7)>>SP_ACCURACY-2),-1,"Settings",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),5*engineWindowY/7+(spSin(menu_counter*300+1*SP_PI*2/7)>>SP_ACCURACY-2),-1,"About",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),6*engineWindowY/7+(spSin(menu_counter*300+0*SP_PI*2/7)>>SP_ACCURACY-2),-1,"Quit",font);
      
      spTranslate(0,1<<SP_ACCURACY-2,3<<SP_ACCURACY-1);
      spTranslate(-5<<SP_ACCURACY,(3<<ACCURACY)-menu_choice*5/2,0);
      draw_particle_circle(-1,menu_counter);
      spTranslate(10<<SP_ACCURACY,0,0);
      draw_particle_circle(+1,menu_counter);

      break;
    case 1: //options
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),1*engineWindowY/8+(spSin(menu_counter*300+6*SP_PI/4)>>SP_ACCURACY-2),-1,"SETTINGS",font);
      switch (settings_get_stone_quality())
      {
        case 0: spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),2*engineWindowY/8+(spSin(menu_counter*300+5*SP_PI/4)>>SP_ACCURACY-2),-1,"Stone Quality: Flat",font); break;
        case 1: spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),2*engineWindowY/8+(spSin(menu_counter*300+5*SP_PI/4)>>SP_ACCURACY-2),-1,"Stone Quality: Border",font); break;
        case 2: spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),2*engineWindowY/8+(spSin(menu_counter*300+5*SP_PI/4)>>SP_ACCURACY-2),-1,"Stone Quality: Texture",font); break;
      }
      switch (settings_get_stars_rotating())
      {
        case 0: spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),3*engineWindowY/8+(spSin(menu_counter*300+4*SP_PI/4)>>SP_ACCURACY-2),-1,"Stars: Off",font); break;
        case 1: spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),3*engineWindowY/8+(spSin(menu_counter*300+4*SP_PI/4)>>SP_ACCURACY-2),-1,"Stars: Still",font); break;
        case 2: spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),3*engineWindowY/8+(spSin(menu_counter*300+4*SP_PI/4)>>SP_ACCURACY-2),-1,"Stars: Rotating",font); break;
      }
      if (settings_get_particles())
        spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),4*engineWindowY/8+(spSin(menu_counter*300+3*SP_PI/4)>>SP_ACCURACY-2),-1,"Particles: On",font);
      else
        spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),4*engineWindowY/8+(spSin(menu_counter*300+3*SP_PI/4)>>SP_ACCURACY-2),-1,"Particles: Off",font);

      if (settings_get_control())
        spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),5*engineWindowY/8+(spSin(menu_counter*300+2*SP_PI/4)>>SP_ACCURACY-2),-1,"Control: Snake",font);
      else
        spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),5*engineWindowY/8+(spSin(menu_counter*300+2*SP_PI/4)>>SP_ACCURACY-2),-1,"Control: Normal",font);

      char buffer[256];
      sprintf(buffer,"Volume %i%%",settings_get_volume());
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),6*engineWindowY/8+(spSin(menu_counter*300+1*SP_PI/4)>>SP_ACCURACY-2),-1,buffer,font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),7*engineWindowY/8+(spSin(menu_counter*300+0*SP_PI/4)>>SP_ACCURACY-2),-1,"Back to Menu",font);

      spTranslate(0,1<<SP_ACCURACY-2,-1<<SP_ACCURACY-1);
      spTranslate(-10<<SP_ACCURACY,(4<<ACCURACY)-menu_choice*5/2,0);
      draw_particle_circle(-1,menu_counter);
      spTranslate(20<<SP_ACCURACY,0,0);
      draw_particle_circle(+1,menu_counter);
      break;
    case 2: //Free Game
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),1*engineWindowY/7+(spSin(menu_counter*300+5*SP_PI/3)>>SP_ACCURACY-2),-1,"FREE GAME",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),2*engineWindowY/7+(spSin(menu_counter*300+1*SP_PI/3)>>SP_ACCURACY-2),-1,"<<< Play >>>",font);
      if (settings_get_mode() == 0)
        spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),3*engineWindowY/7+(spSin(menu_counter*300+4*SP_PI/3)>>SP_ACCURACY-2),-1,"Game Mode: Points",font);
      else
        spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),3*engineWindowY/7+(spSin(menu_counter*300+4*SP_PI/3)>>SP_ACCURACY-2),-1,"Game Mode: Time Stole",font);
      sprintf(buffer,"Different stone kinds: %i",settings_get_color());
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),4*engineWindowY/7+(spSin(menu_counter*300+3*SP_PI/3)>>SP_ACCURACY-2),-1,buffer,font);
      sprintf(buffer,"Difficulty: %i",settings_get_difficult());
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),5*engineWindowY/7+(spSin(menu_counter*300+2*SP_PI/3)>>SP_ACCURACY-2),-1,buffer,font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),6*engineWindowY/7+(spSin(menu_counter*300+0*SP_PI/3)>>SP_ACCURACY-2),-1,"Back to Menu",font);

      spTranslate(0,1<<SP_ACCURACY-2,3<<SP_ACCURACY-1);
      spTranslate(-10<<SP_ACCURACY,(3<<ACCURACY)-menu_choice*5/2,0);
      draw_particle_circle(-1,menu_counter);
      spTranslate(20<<SP_ACCURACY,0,0);
      draw_particle_circle(+1,menu_counter);
      break;
    case 3: //High Score
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),engineWindowY/2+(spSin(menu_counter*300+6*SP_PI*2/7)>>SP_ACCURACY-2),-1,"Under Construction",font);
      break;

    case 4: //About
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),1*engineWindowY/9+(spSin(menu_counter*300+6*SP_PI*2/7)>>SP_ACCURACY-2),-1,"ABOUT",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),2*engineWindowY/9+(spSin(menu_counter*300+5*SP_PI*2/7)>>SP_ACCURACY-2),-1,"Code Monkey: Ziz",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),3*engineWindowY/9+(spSin(menu_counter*300+4*SP_PI*2/7)>>SP_ACCURACY-2),-1,"Sound: Nick May",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),4*engineWindowY/9+(spSin(menu_counter*300+3*SP_PI*2/7)>>SP_ACCURACY-2),-1,"Backers of \"RIOTdigital\":",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),5*engineWindowY/9+(spSin(menu_counter*300+2*SP_PI*2/7)>>SP_ACCURACY-2),-1,"Pandora",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),6*engineWindowY/9+(spSin(menu_counter*300+1*SP_PI*2/7)>>SP_ACCURACY-2),-1,"GPH",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),7*engineWindowY/9+(spSin(menu_counter*300+0*SP_PI*2/7)>>SP_ACCURACY-2),-1,"DINGOO DIGITAL USA",font);
      spFontDrawMiddle(engineWindowX/2+(menu_fade*spGetSizeFactor()>>SP_ACCURACY+2),8*engineWindowY/9+(spSin(menu_counter*300+-1*SP_PI*2/7)>>SP_ACCURACY-2),-1,"Etronixmart",font);
      break;
  }
  #ifdef MENU_DEBUG
    printf("%i:   Drew menu\n",SDL_GetTicks());
  #endif

  char buffer[16];
  sprintf(buffer,"fps: %i",spGetFPS());
  spFontDrawRight(engineWindowX,engineWindowY-small_font->maxheight,-1,buffer,small_font);
  #ifdef MENU_DEBUG
    printf("%i:   Drew fps\n",SDL_GetTicks());
  #endif
  spFlip();
}

int calc_menu(Uint32 steps)
{
  menu_counter += steps;
  calc_music(steps);
  PspInput engineInput = spGetInput();
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
          rotating_sound_off();
          settings_save();
          menu_counter = run_game(1,settings_get_mode(),settings_get_difficult(),menu_counter*10,menu_resize)/10;
          engineInput->button[SP_BUTTON_START] = 0;
          engineInput->button[SP_BUTTON_A] = 0;
          engineInput->button[SP_BUTTON_B] = 0;
          engineInput->button[SP_BUTTON_X] = 0;
          engineInput->button[SP_BUTTON_Y] = 0;
          state = 0;
          break;
        case -4:
          rotating_sound_off();
          return 1;
      }
    }
    return 0;
  }

  int i;
  
  switch (state)
  {
  case 0: //menu
    if (engineInput->axis[1]<0 && menu_move==0 && (menu_choice>>SP_ACCURACY)<4)
      menu_move = 1;
    if (engineInput->axis[1]>0 && menu_move==0 && (menu_choice>>SP_ACCURACY)>0)
      menu_move = -1;
    
    if (menu_move != 0)
    {
      move_sound_on();
      for (i=0;i<steps;i++)
      {
        menu_choice += menu_move*4 << SP_ACCURACY-10;
        if (menu_choice == ((menu_choice >>SP_ACCURACY)<<SP_ACCURACY))
          menu_move = 0;
      }
    }
    else
      move_sound_off();
    if (menu_move == 0 && (engineInput->button[SP_BUTTON_START] ||
        engineInput->button[SP_BUTTON_A] || engineInput->button[SP_BUTTON_B] ||
        engineInput->button[SP_BUTTON_X] || engineInput->button[SP_BUTTON_Y]))
    {
      switch (menu_choice>>SP_ACCURACY)
      {
        case 0: // Free Game
          nextstate = 2;
          menu_fade = -1;
          rotating_sound_on();
          break;
        case 1: // Highscore
          nextstate = 3;
          menu_fade = -1;
          rotating_sound_on();
          break;
        case 2: // Options
          nextstate = 1;
          menu_fade = -1;
          rotating_sound_on();
          break;
        case 3: // About
          nextstate = 4;
          menu_fade = -1;
          rotating_sound_on();
          break;
        case 4: // Quit
          nextstate = -4;
          menu_fade = -1;
          rotating_sound_on();
          break;
      }
    }
    break;
  case 1: //options
    if (engineInput->axis[1]<0 && menu_move==0 && (menu_choice>>SP_ACCURACY)<5)
      menu_move = 1;
    if (engineInput->axis[1]>0 && menu_move==0 && (menu_choice>>SP_ACCURACY)>0)
      menu_move = -1;
    
    if (menu_move != 0)
    {
      move_sound_on();
      for (i=0;i<steps;i++)
      {
        menu_choice += menu_move*4 << SP_ACCURACY-10;
        if (menu_choice == ((menu_choice >>SP_ACCURACY)<<SP_ACCURACY))
          menu_move = 0;
      }
    }
    else
      move_sound_off();
    for (i = 0;i < steps;i++)
    {
      menu_wait--;
      if (menu_move == 0 && (menu_choice>>SP_ACCURACY) == 4 && engineInput->axis[0]<0 && menu_wait <= 0 && settings_get_volume()>0)
      {
        settings_set_volume(settings_get_volume()-1);
        set_volume(settings_get_volume());
        menu_wait = 25;
      }
      if (menu_move == 0 && (menu_choice>>SP_ACCURACY) == 4 && engineInput->axis[0]>0 && menu_wait <= 0 && settings_get_volume()<100)
      {
        settings_set_volume(settings_get_volume()+1);
        set_volume(settings_get_volume());
        menu_wait = 25;
      }
    }
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 0 && engineInput->axis[0]<0 && menu_wait <= 0 && settings_get_stone_quality()>0)
    {
      settings_set_stone_quality(settings_get_stone_quality()-1);
      menu_block = 1;
    }
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 0 && engineInput->axis[0]>0 && menu_wait <= 0 && settings_get_stone_quality()<2)
    {
      settings_set_stone_quality(settings_get_stone_quality()+1);
      menu_block = 1;
    }
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 1 && engineInput->axis[0]<0 && menu_wait <= 0 && settings_get_stars_rotating()>0)
    {
      settings_set_stars_rotating(settings_get_stars_rotating()-1);
      menu_block = 1;
    }
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 1 && engineInput->axis[0]>0 && menu_wait <= 0 && settings_get_stars_rotating()<2)
    {
      settings_set_stars_rotating(settings_get_stars_rotating()+1);
      menu_block = 1;
    }
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 2 && engineInput->axis[0]<0 && menu_wait <= 0 && settings_get_particles()>0)
    {
      settings_set_particles(settings_get_particles()-1);
      menu_block = 1;
    }
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 2 && engineInput->axis[0]>0 && menu_wait <= 0 && settings_get_particles()<1)
    {
      settings_set_particles(settings_get_particles()+1);
      menu_block = 1;
    }
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 3 && engineInput->axis[0]<0 && menu_wait <= 0 && settings_get_control()>0)
    {
      settings_set_control(settings_get_control()-1);
      menu_block = 1;
    }
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 3 && engineInput->axis[0]>0 && menu_wait <= 0 && settings_get_control()<1)
    {
      settings_set_control(settings_get_control()+1);
      menu_block = 1;
    }
    if (engineInput->axis[0] == 0)
      menu_block = 0;
    if (menu_move == 0 && (engineInput->button[SP_BUTTON_START] ||
        engineInput->button[SP_BUTTON_A] || engineInput->button[SP_BUTTON_B] ||
        engineInput->button[SP_BUTTON_X] || engineInput->button[SP_BUTTON_Y]))
    {
      engineInput->button[SP_BUTTON_START] = 0;
      engineInput->button[SP_BUTTON_A] = 0;
      engineInput->button[SP_BUTTON_B] = 0;
      engineInput->button[SP_BUTTON_X] = 0;
      engineInput->button[SP_BUTTON_Y] = 0;
      switch (menu_choice>>SP_ACCURACY)
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
        case 3: //Control
          settings_set_control((settings_get_control()+1)%2);
          break;
        case 5: //Back
          settings_save();
          nextstate = 0;
          menu_fade = -1;
          rotating_sound_on();
          break;
      }
    }
    break;
  case 2: //free game
    if (engineInput->axis[1]<0 && menu_move==0 && (menu_choice>>SP_ACCURACY)<4)
      menu_move = 1;
    if (engineInput->axis[1]>0 && menu_move==0 && (menu_choice>>SP_ACCURACY)>0)
      menu_move = -1;
    
    if (menu_move != 0)
    {
      move_sound_on();
      for (i=0;i<steps;i++)
      {
        menu_choice += menu_move*4 << SP_ACCURACY-10;
        if (menu_choice == ((menu_choice >>SP_ACCURACY)<<SP_ACCURACY))
          menu_move = 0;
      }
    }
    else
      move_sound_off();
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 1 && engineInput->axis[0]!=0 && menu_wait <= 0)
    {
      settings_set_mode((settings_get_mode()+1)%2);
      menu_block = 1;
    }
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 2 && engineInput->axis[0]<0 && menu_wait <= 0 && settings_get_color()>4)
    {
      settings_set_color(settings_get_color()-1);
      menu_block = 1;
    }
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 2 && engineInput->axis[0]>0 && menu_wait <= 0 && settings_get_color()<9)
    {
      settings_set_color(settings_get_color()+1);
      menu_block = 1;
    }
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 3 && engineInput->axis[0]<0 && menu_wait <= 0 && settings_get_difficult()>0)
    {
      settings_set_difficult(settings_get_difficult()-1);
      menu_block = 1;
    }
    if (!menu_block && menu_move == 0 && (menu_choice>>SP_ACCURACY) == 3 && engineInput->axis[0]>0 && menu_wait <= 0 && settings_get_difficult()<9)
    {
      settings_set_difficult(settings_get_difficult()+1);
      menu_block = 1;
    }
    if (engineInput->axis[0] == 0)
      menu_block = 0;
    if (menu_move == 0 && (engineInput->button[SP_BUTTON_START] ||
        engineInput->button[SP_BUTTON_A] || engineInput->button[SP_BUTTON_B] ||
        engineInput->button[SP_BUTTON_X] || engineInput->button[SP_BUTTON_Y]))
    {
      engineInput->button[SP_BUTTON_START] = 0;
      engineInput->button[SP_BUTTON_A] = 0;
      engineInput->button[SP_BUTTON_B] = 0;
      engineInput->button[SP_BUTTON_X] = 0;
      engineInput->button[SP_BUTTON_Y] = 0;
      switch (menu_choice>>SP_ACCURACY)
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
  case 3: //high score
    if (engineInput->button[SP_BUTTON_START] ||
        engineInput->button[SP_BUTTON_A] || engineInput->button[SP_BUTTON_B] ||
        engineInput->button[SP_BUTTON_X] || engineInput->button[SP_BUTTON_Y])
    {
      engineInput->button[SP_BUTTON_START] = 0;
      engineInput->button[SP_BUTTON_A] = 0;
      engineInput->button[SP_BUTTON_B] = 0;
      engineInput->button[SP_BUTTON_X] = 0;
      engineInput->button[SP_BUTTON_Y] = 0;
      nextstate = 0;
      menu_fade = -1;
      rotating_sound_on();
    }
    break;
  case 4: //about
    if (engineInput->button[SP_BUTTON_START] ||
        engineInput->button[SP_BUTTON_A] || engineInput->button[SP_BUTTON_B] ||
        engineInput->button[SP_BUTTON_X] || engineInput->button[SP_BUTTON_Y])
    {
      engineInput->button[SP_BUTTON_START] = 0;
      engineInput->button[SP_BUTTON_A] = 0;
      engineInput->button[SP_BUTTON_B] = 0;
      engineInput->button[SP_BUTTON_X] = 0;
      engineInput->button[SP_BUTTON_Y] = 0;
      nextstate = 0;
      menu_fade = -1;
      rotating_sound_on();
    }
    break;
  }
  return 0;
}

void run_menu(void (*resize)(Uint16 w,Uint16 h))
{
  menu_fade = MENUSIZE;
  menu_choice = 0;
  rotating_sound_on();
  menu_resize = resize;
  spLoop(draw_menu,calc_menu,10,resize);
}

