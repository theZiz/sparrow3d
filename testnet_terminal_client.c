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
	spNetC4AGetScore(&score,profile,"puzzletube_points",10000);
	while (spNetC4AGetStatus() == SP_C4A_PROGRESS)
	#ifdef WIN32
		Sleep(1);
	#else
		usleep(200);
	#endif
	if (spNetC4AGetStatus() == SP_C4A_OK)
	{
		spNetC4AScorePointer mom = score;
		while (mom)
		{
			struct tm * local = localtime (&(mom->commitTime));
			printf("%2i.%2i.%i - %2i:%02i: %s (%s) - %i\n",local->tm_mday,local->tm_mon+1,local->tm_year+1900,local->tm_hour,local->tm_min,mom->longname,mom->shortname,mom->score);
			mom = mom->next;
		}
		//If you just uncomment this code, you cheat! Copy it to your game and try
		//with YOUR game. Thank you. ;)
		//SDL_WaitThread(spNetC4ACommitScore(profile,"puzzletube_points",10003,&score),NULL);
		//printf("Commit end status code: %i\n",spNetC4AGetStatus());
		spNetC4ADeleteScores(&score);
	}
	else
		printf("Fetshing Highscore is still running or failed with status code: %i\n",spNetC4AGetStatus());

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
		printf("No client_connection! I will crash!");
	
	do_stuff();
	
	spNetCloseTCP(client_connection);
	
	//After the loop because spNetC4AGetScore works in the background. ;)

	spQuitNet();
	return 0;
}
