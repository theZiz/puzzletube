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
#ifndef _MUSIC_H
#define _MUSIC_H
#include <SDL/SDL_mixer.h>
#include "../sparrow3d/sparrow3d.h"
void init_music();
void change_music(char* name,char* author);
void draw_music();
void calc_music(int steps);
void quit_music();
void move_sound_on();
void move_sound_off();
void rotating_sound_on();
void rotating_sound_off();
void set_volume(int volume);
void play_explosion();
void play_switch();

#endif
