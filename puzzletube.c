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
 of the GNU Lesser General Public license (the	"LGPL License"), in which case the	
 provisions of LGPL License are applicable instead of those									
 above.																																		 
																																						
 For feedback and questions about my Files and Projects please mail me,		 
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com												 
*/

#include <sparrow3d.h>
#include <stdlib.h>
#include "music.h"
#include "menu.h"
#include "settings.h"
#include "particle.h"
#include "game.h"
#include "splashscreen.h"
#include "c4a.h"

#define FONT_LOCATION "./font/CabinCondensed-Regular.ttf"
#define FONT_SIZE 15
#define FONT_MIDDLE_SIZE 10
#define FONT_SMALL_SIZE 10
#define FONT_COUNTDOWN_SIZE 128
#define FONT_HIGHSCORE_SIZE 48
#define FONT_COLOR_1 65535
#define FONT_COLOR_2 48631
#define FONT_BORDER_1 0
#define FONT_BORDER_2 0

SDL_Surface* screen;
spFontPointer font = NULL;
spFontPointer small_font = NULL;
spFontPointer middle_font = NULL;
spFontPointer countdown_font = NULL;
spFontPointer highscore_font = NULL;

void resize(Uint16 w,Uint16 h)
{
  //Setup of the new/resized window
  spSelectRenderTarget(spGetWindowSurface());
  spSetPerspective(50.0,(float)spGetWindowSurface()->w/(float)spGetWindowSurface()->h,1.0,100);
	spBundlePointer translation = settings_get_translation();
	spFontShadeButtons(1);

	//Font Loading
	spFontSetShadeColor(FONT_BORDER_1);
	if (font)
		spFontDelete(font);
	font = spFontLoad(FONT_LOCATION,FONT_SIZE*spGetSizeFactor()>>SP_ACCURACY);
	spFontAdd(font,SP_FONT_GROUP_ASCII,FONT_COLOR_1);//whole ASCII
	spFontAddEveryLetterOfTextBundle(font,translation,FONT_COLOR_1);
	spFontAddBorder(font,FONT_BORDER_1);
	spFontMulWidth(font,15<<SP_ACCURACY-4);
	spFontAddButton( font, 'A', SP_BUTTON_LEFT_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'B', SP_BUTTON_RIGHT_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'X', SP_BUTTON_DOWN_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'Y', SP_BUTTON_UP_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'L', SP_BUTTON_L_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'R', SP_BUTTON_R_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'S', SP_BUTTON_START_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'E', SP_BUTTON_SELECT_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	settings_set_font(font);

	spFontSetShadeColor(FONT_BORDER_2);
	if (small_font)
		spFontDelete(small_font);
	small_font = spFontLoad(FONT_LOCATION,FONT_SMALL_SIZE*spGetSizeFactor()>>SP_ACCURACY);
	spFontAdd(small_font,SP_FONT_GROUP_ASCII,FONT_COLOR_2);//whole ASCII
	//spFontAdd(small_font,SP_FONT_GROUP_GERMAN,0);//some German letters
	spFontAddEveryLetterOfTextBundle(small_font,translation,FONT_COLOR_2);
	spFontAddBorder(small_font,FONT_BORDER_2);
	spFontMulWidth(small_font,15<<SP_ACCURACY-4);
	spFontAddButton( small_font, 'A', SP_BUTTON_LEFT_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( small_font, 'B', SP_BUTTON_RIGHT_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( small_font, 'X', SP_BUTTON_DOWN_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( small_font, 'Y', SP_BUTTON_UP_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( small_font, 'L', SP_BUTTON_L_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( small_font, 'R', SP_BUTTON_R_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( small_font, 'S', SP_BUTTON_START_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( small_font, 'E', SP_BUTTON_SELECT_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	settings_set_small_font(small_font);

	spFontSetShadeColor(FONT_BORDER_1);
	if (middle_font)
		spFontDelete(middle_font);
	middle_font = spFontLoad(FONT_LOCATION,FONT_MIDDLE_SIZE*spGetSizeFactor()>>SP_ACCURACY);
	spFontAdd(middle_font,SP_FONT_GROUP_ASCII,FONT_COLOR_1);//whole ASCII
	//spFontAdd(middle_font,SP_FONT_GROUP_GERMAN,0);//some German letters
	spFontAddEveryLetterOfTextBundle(middle_font,translation,FONT_COLOR_1);
	spFontAddBorder(middle_font,FONT_BORDER_1);
	spFontMulWidth(middle_font,15<<SP_ACCURACY-4);
	spFontAddButton( middle_font, 'A', SP_BUTTON_LEFT_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( middle_font, 'B', SP_BUTTON_RIGHT_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( middle_font, 'X', SP_BUTTON_DOWN_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( middle_font, 'Y', SP_BUTTON_UP_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( middle_font, 'L', SP_BUTTON_L_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( middle_font, 'R', SP_BUTTON_R_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( middle_font, 'S', SP_BUTTON_START_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( middle_font, 'E', SP_BUTTON_SELECT_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	settings_set_middle_font(middle_font);

	spFontSetShadeColor(FONT_BORDER_1);
	if (countdown_font)
		spFontDelete(countdown_font);
	countdown_font = spFontLoad(FONT_LOCATION,FONT_COUNTDOWN_SIZE*spGetSizeFactor()>>SP_ACCURACY);
	spFontAdd(countdown_font,"0123",FONT_COLOR_1);
	spFontAddBorder(countdown_font,FONT_BORDER_1);
	settings_set_countdown_font(countdown_font);

	spFontSetShadeColor(FONT_BORDER_1);
	if (highscore_font)
		spFontDelete(highscore_font);
	highscore_font = spFontLoad(FONT_LOCATION,FONT_HIGHSCORE_SIZE*spGetSizeFactor()>>SP_ACCURACY);
	spFontAdd(highscore_font,"ABCDEFGHIJKLMNOPQRSTUVWXYZ",FONT_COLOR_1);
	spFontAddBorder(highscore_font,FONT_BORDER_1);
	settings_set_highscore_font(highscore_font);

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
	
	//Language setup
	spReadPossibleLanguages("./translations/languages.txt");
	spBundlePointer translation = spLoadBundle("./translations/translations.txt",1);
	/* //counting all hashes:
	spTextPointer mom = translation->firstText;
	int mesh[257];
	memset(mesh,0,257*4);
	int count = 0;
	while (mom)
	{
		if (mom->hash >= 0 && mom->hash < 256)
		{
			mesh[mom->hash]++;
		}
		else
			mesh[256]++;
		count++;
		mom = mom->next;
	}
	int i,j;
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 16; j++)
		{
			printf(" %3i",mesh[i+j*16]);
		}
		printf("\n");
	}
	printf("Invalid hash: %i\n",mesh[256]);
	printf("Total hash: %i\n",count);*/
	settings_set_translation(translation);

	//sparrow3D Init
	//spSetDefaultWindowSize( 800, 480 );
	spInitCore();
	spInitNet();
	spSetAffineTextureHack(0); //We don't need it :)
	spInitMath();
	
	//Setup
	screen = spCreateDefaultWindow();
	resize(screen->w,screen->h);
	
	run_splashscreen(resize);
	spSetCulling(0);
	init_music();
	prepare_game_objects(1);
	change_music("Midnight Mediation","Nick May");
	highscore_save();
	init_c4a();
	run_menu(resize);
	quit_c4a();
	delete_game_objects();
	spDeleteBundle(translation,0);
	spFontDelete(font);
	spFontDelete(small_font);
	spFontDelete(middle_font);
	quit_music();
	spQuitNet();
	spQuitCore();
	return 0;
}
