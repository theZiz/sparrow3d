/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include <sparrowNet.h>
#include <stdio.h>
#ifndef _WIN32
	#include <unistd.h>
#endif

spNetTCPConnection client_connection;

void do_stuff()
{
	char command[256];
	printf("Enter a message and press Return to send. Enter \"exit\" to exit.\n");
	while (1)
	{
		int ret = scanf("%s",command);
		if (strcmp(command,"exit") == 0)
			return;
		else
			spNetSendHTTP(client_connection,command);
	}
}

int main( int argc, char **argv )
{
	spInitNet();
	printf("Init spNet\n");

	//C4A Example
	spNetC4AScorePointer score;
	spNetC4AProfilePointer profile = spNetC4AGetProfile();
	if (profile)
	{
		printf("Your profile:\n");
		printf("Long Name: %s\n",profile->longname);
		printf("Short Name: %s\n",profile->shortname);
		printf("E-Mail: %s\n",profile->email);
		printf("Password: %s\n",profile->password);
	}
	else
		printf("No profile found. Put it to this folder or create it with compo4all!\n");
	
	//spNetC4AGetScoreOfMonth(&score,profile,"puzzletube_points",2013,6);
	printf("Getting Scores serial:\n");
	int i;
	int result = 0;
	for (i = 0; i < 2; i++)
	{
		switch (i)
		{
			case 0:
				result = spNetC4AGetScore(&score,profile,"snowman_hard",10000);
				printf("Highscore of Snowman Hard (with profile if available):\n");
				break;
			case 1:
				result = spNetC4AGetScore(&score,NULL,"snowman_easy",10000);
				printf("Highscore of Snowman Easy (wihtout profile):\n");
				break;
		}
		if (result == 0)
			while (spNetC4AGetStatus() == SP_C4A_PROGRESS)
			#ifdef _WIN32
				Sleep(1);
			#else
				usleep(200);
			#endif
		if (result == 0 && spNetC4AGetStatus() == SP_C4A_OK)
		{
			spNetC4AScorePointer mom = score;
			while (mom)
			{
				struct tm * local = localtime (&(mom->commitTime));
				printf("  %2i: %2i.%2i.%i - %2i:%02i: %s (%s) - %i\n",mom->rank,local->tm_mday,local->tm_mon+1,local->tm_year+1900,local->tm_hour,local->tm_min,mom->longname,mom->shortname,mom->score);
				mom = mom->next;
			}
			printf("  Same with every player just once:\n");
			spNetC4AMakeScoresUnique(&score);
			mom = score;
			while (mom)
			{
				struct tm * local = localtime (&(mom->commitTime));
				printf("  %2i: %2i.%2i.%i - %2i:%02i: %s (%s) - %i\n",mom->rank,local->tm_mday,local->tm_mon+1,local->tm_year+1900,local->tm_hour,local->tm_min,mom->longname,mom->shortname,mom->score);
				mom = mom->next;
			}
			spNetC4ADeleteScores(&score);
		}
		else
		if (result == 1)
			printf("Fetshing Highscore failed with error code: %i\n",result);
		else
			printf("Fetshing Highscore failed with status code: %i\n",spNetC4AGetStatus());
	}




	printf("Getting Scores parallel:\n");
	spNetC4AScorePointer score1;
	spNetC4AScorePointer score2;
	spNetC4AScorePointer score3;
	spNetC4ATaskPointer p1 = spNetC4AGetScoreParallel(&score1,profile,"puzzletube_points",10000);
	spNetC4ATaskPointer p2 = spNetC4AGetScoreParallel(&score2,NULL,"puzzletube_race",10000);
	spNetC4ATaskPointer p3 = spNetC4AGetScoreParallel(&score3,NULL,"puzzletube_survival",10000);
	if (p1 && p2 && p3)
	while (spNetC4AGetStatusParallel(p1) == SP_C4A_PROGRESS ||
			spNetC4AGetStatusParallel(p2) == SP_C4A_PROGRESS ||
			spNetC4AGetStatusParallel(p3) == SP_C4A_PROGRESS)
	#ifdef _WIN32
		Sleep(1);
	#else
		usleep(200);
	#endif
	for (i = 0; i < 3; i++)
	{
		spNetC4ATaskPointer p;
		switch (i)
		{
			case 0:
				printf("Highscore of Puzzletube Points (with profile if available):\n");
				p = p1;
				score = score1;
				break;
			case 1:
				printf("Highscore of Puzzletube Race (wihtout profile):\n");
				p = p2;
				score = score2;
				break;
			case 2:
				printf("Highscore of Puzzletube Survival (without profile):\n");
				p = p3;
				score = score3;
				break;
		}
		if (p && spNetC4AGetStatusParallel(p) == SP_C4A_OK)
		{
			spNetC4AScorePointer mom = score;
			while (mom)
			{
				struct tm * local = localtime (&(mom->commitTime));
				printf("  %2i: %2i.%2i.%i - %2i:%02i: %s (%s) - %i\n",mom->rank,local->tm_mday,local->tm_mon+1,local->tm_year+1900,local->tm_hour,local->tm_min,mom->longname,mom->shortname,mom->score);
				mom = mom->next;
			}
			spNetC4ADeleteScores(&score);
		}
		else
		if (p == NULL)
			printf("Fetshing Highscore failed with error code: 0\n");
		else
			printf("Fetshing Highscore failed with status code: %i\n",spNetC4AGetStatusParallel(p));
		spNetC4ADeleteTask(p);
	}
	//Client setup
	spNetIP ip;
	if (argc < 2)
	{
		ip = spNetResolve("localhost",12345);
		printf("Connection to localhost\nUse ./testnet.sh SERVER to specify another server to connect to!\n");
	}
	else
	{
		printf("Connecting to %s\n",argv[1]);
		ip = spNetResolve(argv[1],12345);
	}
	client_connection = spNetOpenClientTCP(ip);
	if (client_connection == NULL)
		printf("No client_connection! I will crash!\n");
	
	do_stuff();
	
	spNetCloseTCP(client_connection);
	
	//After the loop because spNetC4AGetScore works in the background. ;)

	spQuitNet();
	return 0;
}
