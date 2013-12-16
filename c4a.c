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
#include "c4a.h"
#include "settings.h"

#define TIME_OUT 15000

char c4aStatus[256] = "";
//int c4aFade;
spNetC4AScorePointer raceScore=NULL,pointsScore=NULL,survivalScore=NULL;
spNetC4AProfilePointer profile;
int c4aState = 0;
int c4aErrorTimeOut = 0;

void init_c4a()
{
	spInitNet();
	profile = spNetC4AGetProfile();
	if (profile == NULL)
	{
		sprintf(c4aStatus,"C4A-Manager or Mini-C4A not installed.");
		c4aErrorTimeOut = 20000;
		c4aState = -3;
	}
	updateScores();
}

void updateScores()
{
	if (profile == NULL)
		return;
	int status = spNetC4AGetStatus();
	if (status > 0)
		spNetC4ACancelTask();
	spNetC4ADeleteScores(&pointsScore);
	spNetC4ADeleteScores(&survivalScore);
	spNetC4ADeleteScores(&raceScore);
	c4aState = 1; //update
	spNetC4AGetScore(&pointsScore,profile,"puzzletube_points",TIME_OUT);
}

int score_col = 0;
int score_row = 0;

void init_score_commit()
{
	switch (score_col)
	{
		case 0:
			if (get_highscore(0,1,score_row) == 0 || spNetC4ACommitScore(profile,"puzzletube_points",get_highscore(0,1,score_row),&pointsScore,TIME_OUT))
				printf("Points score %i already at server\n",get_highscore(0,1,score_row));
			break;
		case 1:
			if (get_highscore(1,1,score_row) == 0 || spNetC4ACommitScore(profile,"puzzletube_survival",get_highscore(1,1,score_row)*100,&survivalScore,TIME_OUT))
				printf("Survival score %i already at server\n",get_highscore(1,1,score_row));
			break;
		case 2:
			if (get_highscore(2,1,score_row) == 10000 || spNetC4ACommitScore(profile,"puzzletube_race",get_highscore(2,1,score_row)*100,&raceScore,TIME_OUT))
				printf("Race score %i already at server\n",get_highscore(2,1,score_row));
			break;
	}
}

void init_one_score_commit(int gametype,int score)
{
	if (settings_get_color() == 0)
		return;
	score_col = gametype;
	switch (gametype)
	{
		case 0:
			if (spNetC4ACommitScore(profile,"puzzletube_points",score,&pointsScore,TIME_OUT))
				printf("Points score %i already at server\n",score);
			else
				c4aState = 5;
			break;
		case 1:
			if (spNetC4ACommitScore(profile,"puzzletube_survival",score*100,&survivalScore,TIME_OUT))
				printf("Survival score %i already at server\n",score);
			else
				c4aState = 5;
			break;
		case 2:
			if (spNetC4ACommitScore(profile,"puzzletube_race",score*100,&raceScore,TIME_OUT))
				printf("Race score %i already at server\n",score);
				c4aState = 5;
			break;
	}
}

void send_c4a_scores(int timeout_sec,int timeout_dec)
{
	switch (spNetC4AGetStatus())
	{
		case SP_C4A_PROGRESS:
			switch (score_col)
			{
				case 0:
					sprintf(c4aStatus,"Commit points scores. %i.%is",timeout_sec,timeout_dec);
					break;
				case 1:
					sprintf(c4aStatus,"Commit survival scores. %i.%is",timeout_sec,timeout_dec);
					break;
				case 2:
					sprintf(c4aStatus,"Commit race scores. %i.%is",timeout_sec,timeout_dec);
					break;
			}
			break;
		case SP_C4A_ERROR:
			sprintf(c4aStatus,"Error with connection");
			c4aErrorTimeOut = 20000;
			c4aState = -1;
			break;
		case SP_C4A_TIMEOUT:
			sprintf(c4aStatus,"Connection timeout");
			c4aErrorTimeOut = 20000;
			c4aState = -1;
			break;
		case SP_C4A_OK:
			score_row++;
			if (score_row >= 3)
			{
				score_row = 0;
				score_col++;
				if (score_col >= 3)
				{
					c4aState = 0;
					break;
				}
			}
			init_score_commit();
			c4aState = 4;
			break;
	}
}

void send_one_c4a_scores(int timeout_sec,int timeout_dec)
{
	switch (spNetC4AGetStatus())
	{
		case SP_C4A_PROGRESS:
			switch (score_col)
			{
				case 0:
					sprintf(c4aStatus,"Commit points scores. %i.%is",timeout_sec,timeout_dec);
					break;
				case 1:
					sprintf(c4aStatus,"Commit survival scores. %i.%is",timeout_sec,timeout_dec);
					break;
				case 2:
					sprintf(c4aStatus,"Commit race scores. %i.%is",timeout_sec,timeout_dec);
					break;
			}
			break;
		case SP_C4A_ERROR:
			sprintf(c4aStatus,"Error with connection");
			c4aErrorTimeOut = 20000;
			c4aState = -1;
			break;
		case SP_C4A_TIMEOUT:
			sprintf(c4aStatus,"Connection timeout");
			c4aErrorTimeOut = 20000;
			c4aState = -1;
			break;
		case SP_C4A_OK:
			c4aState = 0;
			break;
	}
}

void calc_c4a(int steps)
{
	int timeout_sec;
	int timeout_dec;
	if (c4aErrorTimeOut > 0)
		c4aErrorTimeOut -= steps;
	if (profile == NULL)
		return;
	if (c4aState > 0)
	{
		timeout_sec = spNetC4AGetTimeOut() / 1000;
		timeout_dec = (spNetC4AGetTimeOut() / 100) % 10;
	}
	switch (c4aState)
	{
		case 1:
			switch (spNetC4AGetStatus())
			{
				case SP_C4A_PROGRESS:
					sprintf(c4aStatus,"Loading points highscore. %i.%is",timeout_sec,timeout_dec);
					break;
				case SP_C4A_ERROR:
					sprintf(c4aStatus,"Error with connection");
					c4aErrorTimeOut = 20000;
					c4aState = -1;
					break;
				case SP_C4A_TIMEOUT:
					sprintf(c4aStatus,"Connection timeout");
					c4aErrorTimeOut = 20000;
					c4aState = -1;
					break;
				case SP_C4A_OK:
					spNetC4AGetScore(&survivalScore,profile,"puzzletube_survival",TIME_OUT);
					c4aState = 2;
					break;
			}
			break;
		case 2:
			switch (spNetC4AGetStatus())
			{
				case SP_C4A_PROGRESS:
					sprintf(c4aStatus,"Loading survival highscore. %i.%is",timeout_sec,timeout_dec);
					break;
				case SP_C4A_ERROR:
					sprintf(c4aStatus,"Error with connection");
					c4aErrorTimeOut = 20000;
					c4aState = -1;
					break;
				case SP_C4A_TIMEOUT:
					sprintf(c4aStatus,"Connection timeout");
					c4aErrorTimeOut = 20000;
					c4aState = -1;
					break;
				case SP_C4A_OK:
					spNetC4AGetScore(&raceScore,profile,"puzzletube_race",TIME_OUT);
					c4aState = 3;
					break;
			}
			break;
		case 3:
			switch (spNetC4AGetStatus())
			{
				case SP_C4A_PROGRESS:
					sprintf(c4aStatus,"Loading race highscore. %i.%is",timeout_sec,timeout_dec);
					break;
				case SP_C4A_ERROR:
					sprintf(c4aStatus,"Error with connection");
					c4aErrorTimeOut = 20000;
					c4aState = -1;
					break;
				case SP_C4A_TIMEOUT:
					sprintf(c4aStatus,"Connection timeout");
					c4aErrorTimeOut = 20000;
					c4aState = -1;
					break;
				case SP_C4A_OK:
					score_row = 0;
					score_col = 0;
					init_score_commit();
					c4aState = 4;
					break;
			}
			break;
		case 4:
			send_c4a_scores(timeout_sec,timeout_dec);
			break;
		case 5:
			send_one_c4a_scores(timeout_sec,timeout_dec);
			break;
	}
}

void draw_c4a()
{
	if (c4aState == 0)
		return;
	if (spNetC4AGetTimeOut() <= 0 && c4aErrorTimeOut <= 0)
		return;
	spFontPointer small_font = settings_get_small_font();
	int engineWindowX=spGetWindowSurface()->w;
	int engineWindowY=spGetWindowSurface()->h;
	spFontDraw(1,engineWindowY-(small_font->maxheight),-1,c4aStatus,small_font);
}

void quit_c4a()
{
	int status = spNetC4AGetStatus();
	if (status != SP_C4A_OK && status != SP_C4A_ERROR)
		spNetC4ACancelTask();
	if (profile == NULL)
		return;
	spNetC4AFreeProfile(profile);
	spNetC4ADeleteScores(&pointsScore);
	spNetC4ADeleteScores(&survivalScore);
	spNetC4ADeleteScores(&raceScore);
}
