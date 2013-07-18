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

#include "lettering.h"
#include <stdlib.h>
#include <string.h>

plettering first_lettering = NULL;

void add_lettering(char* text,spFontPointer font)
{
	//Create
	plettering lettering = (plettering)malloc(sizeof(tlettering));
	lettering->text = (char*)malloc(strlen(text)+1);
	sprintf(lettering->text,"%s",text);
	lettering->age = LETTERING_TIMEOUT;
	lettering->y = spGetWindowSurface()->h/20;
	lettering->font = font;
	lettering->next = first_lettering;
	first_lettering = lettering;
	
	//Recalculate the y-values
	lettering = first_lettering->next;
	while (lettering)
	{
		lettering->y += first_lettering->font->maxheight;
		lettering = lettering->next;
	}
}

void add_line()
{
	//Create
	plettering lettering = (plettering)malloc(sizeof(tlettering));
	lettering->text = NULL;
	lettering->age = LETTERING_TIMEOUT;
	lettering->y = spGetWindowSurface()->h/16;
	lettering->font = NULL;
	lettering->next = first_lettering;
	first_lettering = lettering;
	
	//Recalculate the y-values
	lettering = first_lettering->next;
	while (lettering)
	{
		lettering->y += spGetWindowSurface()->h/32;
		lettering = lettering->next;
	}
}

void calc_lettering(int steps)
{
	plettering lettering = first_lettering;
	plettering last_lettering = NULL;
	//Adding
	while (lettering)
	{
		lettering->age -= steps;
		if (lettering->age<=0 || lettering->y > spGetWindowSurface()->h)
			break;
		last_lettering = lettering;
		lettering = lettering->next;
	}
	//Deleting if too old or to low (y)
	while (lettering)
	{
		plettering next = lettering->next;
		if (lettering->text)
			free(lettering->text);
		free(lettering);
		lettering = next;
	}
	if (last_lettering)
		last_lettering->next = NULL;
	else
		first_lettering = NULL;
}

void delete_all_lettering()
{
	while (first_lettering)
	{
		plettering next = first_lettering->next;
		if (first_lettering->text)
			free(first_lettering->text);
		free(first_lettering);
		first_lettering = next;
	}
}

plettering get_first_lettering()
{
	return first_lettering;
}

pbordering first_bordering = NULL;

#define DO_BORDERING_STUFF(line,x,y) \
	if (line[(y)][(x)] == -1) \
		line[(y)][(x)] = color; \
	else \
		line[(y)][(x)] = -1;

//#define DO_BORDERING_STUFF(line,x,y) line[(y)][(x)] = color;

void add_bordering(pwinsituation winsituation,Uint16 color)
{
	//Create
	pbordering bordering = (pbordering)malloc(sizeof(tbordering));
	bordering->age = BORDERING_TIMEOUT;
	bordering->alpha = SP_ONE;
	bordering->next = first_bordering;
	first_bordering = bordering;
	//Marking the hor & ver lines out of the winsituation
	char was_already[7][16];
	memset(was_already,0,7*16);
	int x,y;
	for (x = 0; x < 16; x++)
	{
		for (y = 0; y < 7; y++)
		{
			bordering->horizental_line[y][x] = -1;
			bordering->vertical_line[y][x] = -1;
		}
		bordering->horizental_line[7][x] = -1;
	}
	while (winsituation)
	{
		if (!was_already[winsituation->y][winsituation->x])
		{
			DO_BORDERING_STUFF(bordering->horizental_line,winsituation->x,winsituation->y);
			DO_BORDERING_STUFF(bordering->vertical_line,winsituation->x,winsituation->y);
			DO_BORDERING_STUFF(bordering->horizental_line,winsituation->x,winsituation->y+1);
			int x_plus_one = winsituation->x+1;
			if (x_plus_one >= 16)
				x_plus_one-=16;
			DO_BORDERING_STUFF(bordering->vertical_line,x_plus_one,winsituation->y);
			was_already[winsituation->y][winsituation->x] = 1;
		}
		else
			printf("Double! That shouldn't be: %i:%i\n",winsituation->x,winsituation->y);
		winsituation = winsituation->next;
	}
}

void calc_bordering(int steps)
{
	pbordering bordering = first_bordering;
	pbordering last_bordering = NULL;
	//Adding
	while (bordering)
	{
		bordering->age -= steps;
		bordering->alpha = bordering->age*SP_ONE/BORDERING_TIMEOUT;
		if (bordering->age<=0)
			break;
		last_bordering = bordering;
		bordering = bordering->next;
	}
	//Deleting if too old or to low (y)
	while (bordering)
	{
		pbordering next = bordering->next;
		free(bordering);
		bordering = next;
	}
	if (last_bordering)
		last_bordering->next = NULL;
	else
		first_bordering = NULL;
}

void delete_all_bordering()
{
	while (first_bordering)
	{
		pbordering next = first_bordering->next;
		free(first_bordering);
		first_bordering = next;
	}
}

pbordering get_first_bordering()
{
	return first_bordering;
}
