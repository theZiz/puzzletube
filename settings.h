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
#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <sparrow3d.h>

int get_highscore_trophy(int game_mode,int dificult,int points);
int get_next_highscore_trophy(int game_mode,int dificult,int points);

void settings_load();
void settings_save();

int settings_get_stone_quality();
int settings_get_stars_rotating();
int settings_get_particles();
int settings_get_volume();
int settings_get_color();
int settings_get_mode();
int settings_get_control();
int settings_get_language();
int settings_get_borders();

void settings_set_stone_quality(int value);
void settings_set_stars_rotating(int value);
void settings_set_particles(int value);
void settings_set_volume(int value);
void settings_set_color(int value);
void settings_set_mode(int value);
void settings_set_control(int value);
void settings_set_language(int language);
void settings_set_borders(int borders);

char* get_highscore_name(int game_mode,int difficult,int rank);
int get_highscore(int game_mode,int difficult,int rank);
void delete_third_place();
void insert_highscore(int game_mode,int difficult,char* name,int points);

spFontPointer settings_get_font();
void settings_set_font(spFontPointer font);
spFontPointer settings_get_small_font();
void settings_set_small_font(spFontPointer small_font);
spFontPointer settings_get_middle_font();
void settings_set_middle_font(spFontPointer middle_font);
spFontPointer settings_get_countdown_font();
void settings_set_countdown_font(spFontPointer countdown_font);
spFontPointer settings_get_highscore_font();
void settings_set_highscore_font(spFontPointer highscore_font);

void settings_reset_highscore_name(char* name);
void settings_set_highscore_name(char* name);

int settings_get_first_start();

void settings_set_translation(spBundlePointer translation);
spBundlePointer settings_get_translation();

void highscore_save();
void highscore_load();
#endif
