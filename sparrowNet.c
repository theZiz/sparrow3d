/*
 The contents of this file are subject to the Mozilla Public License
 Version 1.1 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 License for the specific language governing rights and limitations
 under the License.

 Alternatively, the contents of this file may be used under the terms of the
 GNU Lesser General Public license (the  "LGPL License") version 2 or higher, in
 which case the provisions of LGPL License are applicable instead of those above
 
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "sparrowNet.h"
#include <stdio.h>

SDL_mutex* spNetC4AStatusMutex;

PREFIX void spInitNet()
{
	if(SDLNet_Init()==-1)
		printf("SDLNet_Init: %s\n", SDLNet_GetError());
	spNetC4AStatusMutex = SDL_CreateMutex();
}

void fill_ip_struct(spNetIPPointer ip)
{
	ip->type = IPV4;
	ip->address.ipv4 = ip->sdl_address.host; //bytes are set automaticly, yeah!
	ip->port = ip->sdl_address.port;	
}

PREFIX spNetIP spNetResolve(char* host,Uint16 port)
{
	spNetIP result;
	SDLNet_ResolveHost(&(result.sdl_address), host, port);
	fill_ip_struct(&result);
	return result;
}

PREFIX char* spNetResolveHost(spNetIP ip,char* host,int host_len)
{
	const char* sdlHost = SDLNet_ResolveIP(&(ip.sdl_address));
	if (strlen(sdlHost) >= host_len)
	{
		host = NULL;
		return NULL;
	}
	sprintf(host,"%s",sdlHost);
	return host;
}

PREFIX spNetTCPConnection spNetOpenClientTCP(spNetIP ip)
{
	spNetTCPConnection result;

	result=SDLNet_TCP_Open(&(ip.sdl_address));
	if(!result) {
		printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		return NULL;
	}
}

PREFIX spNetTCPServer spNetOpenServerTCP(Uint16 port)
{
	IPaddress ip;
	spNetTCPServer result;

	if(SDLNet_ResolveHost(&ip,NULL,port)==-1) {
		printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		return NULL;
	}
	result=SDLNet_TCP_Open(&ip);
	if(!result) {
		printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		return NULL;
	}
}

PREFIX spNetTCPConnection spNetAcceptTCP(spNetTCPServer server)
{
	return SDLNet_TCP_Accept(server);
}

PREFIX spNetIP spNetGetConnectionIP(spNetTCPConnection connection)
{
	IPaddress *temp_ip;
	temp_ip=SDLNet_TCP_GetPeerAddress(connection);
	spNetIP result;
	if(!temp_ip) {
		printf("SDLNet_TCP_GetPeerAddress: %s\n", SDLNet_GetError());
		printf("This may be a server socket.\n");
		printf("However, the ip may not be valid!\n");
	}
	result.sdl_address = *temp_ip;
	fill_ip_struct(&result);
	return result;
}

PREFIX int spNetSendTCP(spNetTCPConnection connection,void* data,int length)
{
	return SDLNet_TCP_Send(connection,data,length);
}

PREFIX int spNetSendHTTP(spNetTCPConnection connection,char* data)
{
	return spNetSendTCP(connection,data,strlen(data)+1);
}

typedef struct receivingStruct *receivingPointer;
typedef struct receivingStruct {
	spNetTCPConnection connection;
	void* data;
	int length;
	SDL_mutex* mutex;
	int done;
	SDL_Thread* thread;
	int result;
	receivingPointer next;
} receivingType;

int tcpReceiveThread(void* data)
{
	receivingPointer tcpData = (receivingPointer)data;
	int res=spNetReceiveTCP(tcpData->connection,tcpData->data,tcpData->length);
	SDL_mutexP(tcpData->mutex);
	tcpData->done = 1;
	tcpData->result = res;
	SDL_mutexV(tcpData->mutex);
	return res;
}

int tcpReceiveThread_http(void* data)
{
	receivingPointer tcpData = (receivingPointer)data;
	int res=spNetReceiveHTTP(tcpData->connection,tcpData->data,tcpData->length);
	SDL_mutexP(tcpData->mutex);
	tcpData->done = 1;
	tcpData->result = res;
	SDL_mutexV(tcpData->mutex);
	return res;
}

receivingPointer firstReceiving = NULL;

SDL_Thread* allreadyReceiving(spNetTCPConnection connection)
{
	receivingPointer before = NULL;
	receivingPointer mom = firstReceiving;
	while (mom)
	{
		if (mom->connection == connection)
		{
			SDL_mutexP(mom->mutex);
			if (mom->done)
			{
				SDL_mutexV(mom->mutex); //The Thread lost the interest on this struct
				//Removing mom
				if (before)
				{
					SDL_mutexP(before->mutex);
					before->next = mom->next;
					SDL_mutexV(before->mutex);
				}
				else
					firstReceiving = mom->next;
				SDL_DestroyMutex(mom->mutex);
				if (mom->result<=0) //connection destroyed!
				{
					free(mom);
					return (void*)(-1);
				}
				free(mom);
				return NULL;
			}
			SDL_mutexV(mom->mutex);
			return mom->thread;
		}
		before = mom;
		mom = mom->next;
	}	
	return NULL;
}

PREFIX int spNetReceiveTCP(spNetTCPConnection connection,void* data,int length)
{
	char* data_pointer = (char*)data;
	return SDLNet_TCP_Recv(connection,&(data_pointer[0]),length);
}

PREFIX SDL_Thread* spNetReceiveTCPUnblocked(spNetTCPConnection connection,void* data,int length)
{
	SDL_Thread* thread;
	if (thread = allreadyReceiving(connection))
		return thread;
	receivingPointer tcpData = (receivingPointer)malloc(sizeof(receivingType));
	tcpData->connection = connection;
	tcpData->data = data;
	tcpData->length = length;
	tcpData->connection = connection;
	tcpData->done = 0;
	tcpData->mutex = SDL_CreateMutex();
	tcpData->next = firstReceiving;
	firstReceiving = tcpData;
	tcpData->thread = SDL_CreateThread(tcpReceiveThread,tcpData);
	return tcpData->thread;
}

PREFIX int spNetReceiveHTTP(spNetTCPConnection connection,char* data,int length)
{
	int received = 0;
	while (length > 0)
	{
		int new_received = spNetReceiveTCP(connection,&(data[received]),length);
		received+=new_received;
		length-=new_received;
		if (new_received == 0)
			return received;
	}
	return received;
}

PREFIX SDL_Thread* spNetReceiveHTTPUnblocked(spNetTCPConnection connection,char* data,int length)
{
	SDL_Thread* thread;
	if (thread = allreadyReceiving(connection))
		return thread;
	receivingPointer tcpData = (receivingPointer)malloc(sizeof(receivingType));
	tcpData->connection = connection;
	tcpData->data = data;
	tcpData->length = length;
	tcpData->connection = connection;
	tcpData->done = 0;
	tcpData->mutex = SDL_CreateMutex();
	tcpData->next = firstReceiving;
	firstReceiving = tcpData;
	tcpData->thread = SDL_CreateThread(tcpReceiveThread_http,tcpData);
	return tcpData->thread;
}

PREFIX int spNetReceiveStillWaiting(SDL_Thread* thread)
{
	receivingPointer before = NULL;
	receivingPointer mom = firstReceiving;
	while (mom)
	{
		if (mom->thread == thread)
		{
			SDL_mutexP(mom->mutex);
			if (mom->done)
			{
				SDL_mutexV(mom->mutex); //The Thread lost the interest on this struct
				//Removing mom
				if (before)
				{
					SDL_mutexP(before->mutex);
					before->next = mom->next;
					SDL_mutexV(before->mutex);
				}
				else
					firstReceiving = mom->next;
				SDL_DestroyMutex(mom->mutex);
				free(mom);
				return 0;
			}
			SDL_mutexV(mom->mutex);
			return 1;
		}
		before = mom;
		mom = mom->next;
	}	
	return 0;
}

PREFIX void spNetCloseTCP(spNetTCPConnection connection)
{
	SDLNet_TCP_Close(connection);
}

PREFIX void spQuitNet()
{
	SDL_DestroyMutex(spNetC4AStatusMutex);
	SDLNet_Quit();
}


#ifdef PANDORA
	#include "pnd_locate.h"
#endif

int spNetC4AStatus = 0;
typedef struct getscoreStruct *getscorePointer;
typedef struct getscoreStruct {
	spNetC4AScorePointer* score;
	spNetC4AProfilePointer profile;
	int year;
	int month;
	char game[256];
} getscoreType;

typedef struct commitStruct *commitPointer;
typedef struct commitStruct {
	spNetC4AProfilePointer profile;
	char game[256];
	int score;
	char system[256];
	spNetC4AScorePointer* scoreList;
} commitType;

char* my_strchr(char* buffer, char c, char ignore)
{
	int i;
	int in_ignore = 0;
	for (i = 0; buffer[i]!=0; i++)
	{
		if (buffer[i] == ignore)
			in_ignore = 1-in_ignore;
		if (!in_ignore && buffer[i] == c)
			return &(buffer[i]);
	}
	return NULL;
}

void fill_between_paraphrases(char* buffer, char* dest, int max_size)
{
	int i,j = 0;
	int in_paraphrases = 0;
	for (i = 0; buffer[i]!=0; i++)
	{
		if (buffer[i] == '\"')
		{
			switch (in_paraphrases)
			{
				case 0: in_paraphrases = 1; break;
				case 1: dest[j]=0;return;
			}
			continue;
		}
		if (in_paraphrases)
		{
			dest[j] = buffer[i];
			j++;
			if (j == max_size)
			{
				dest[j-1] = 0;
				return;
			}
		}
	}
}

PREFIX spNetC4AProfilePointer spNetC4AGetProfile()
{
	spNetC4AProfilePointer profile = NULL;
	#ifdef PANDORA
		char *filename = pnd_locate_filename ( "/media/*/pandora/appdata/c4a-mame/:.", "c4a-prof" );
	#else
		char filename[256] = "./c4a-prof";
	#endif
	//Parsing the file
	SDL_RWops *file=SDL_RWFromFile(filename,"rb");
	if (file == NULL)
		return NULL;
	profile = (spNetC4AProfilePointer)malloc(sizeof(spNetC4AProfile));
	char buffer[2048];
	spReadOneLine(file,buffer,2048);
	spReadOneLine(file,buffer,2048);
	char* pos = strstr( buffer, "\"longname\":");
	pos+=11;
	fill_between_paraphrases( pos, profile->longname, 256);
	
	pos = strstr( buffer, "\"shortname\":");
	pos+=12;
	fill_between_paraphrases( pos, profile->shortname, 256);
	
	pos = strstr( buffer, "\"prid\":");
	pos+=7;
	fill_between_paraphrases( pos, profile->prid, 256);

	pos = strstr( buffer, "\"email\":");
	pos+=8;
	fill_between_paraphrases( pos, profile->email, 256);

	pos = strstr( buffer, "\"password\":");
	pos+=11;
	fill_between_paraphrases( pos, profile->password, 256);
	SDL_RWclose(file);
	return profile;
}

PREFIX void spNetC4AFreeProfile(spNetC4AProfilePointer profile)
{
	if (profile)
		free(profile);
}

int c4a_getscore_thread(void* data)
{
	SDL_mutexP(spNetC4AStatusMutex);
	spNetC4AStatus = SP_C4A_ESTABLISHING;
	SDL_mutexV(spNetC4AStatusMutex);
	getscorePointer scoreData = (getscorePointer)data;
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		free(data);
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	SDL_mutexP(spNetC4AStatusMutex);
	spNetC4AStatus = SP_C4A_PROGRESS;
	SDL_mutexV(spNetC4AStatusMutex);
	char get_string[512];
	if (scoreData->year && scoreData->month)
		sprintf(get_string,"GET /json_1/%s/%i%02i/\n\n",scoreData->game,scoreData->year,scoreData->month);
	else
		sprintf(get_string,"GET /json_1/%s\n\n",scoreData->game);
	if (spNetSendHTTP(connection,get_string) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	char buffer[50001]; //skeezix saves the top500. So 100 byte should be enough...
	int length;
	if ((length = spNetReceiveHTTP(connection,buffer,50000)) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	buffer[length] = 0;
	spNetCloseTCP(connection);
	//Searching the first [
	char* found = strchr( buffer, '[' );
	if (found == NULL)
	{
		free(data);
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	//Reading score by score
	//Searching the starting {
	spNetC4AScorePointer lastScore = NULL;
	while (found)
	{
		char* start = strchr( found, '{' );
		if (start == NULL)
		{
			free(data);
			SDL_mutexP(spNetC4AStatusMutex);
			spNetC4AStatus = SP_C4A_ERROR;
			SDL_mutexV(spNetC4AStatusMutex);
			return 1;
		}
		char* end = my_strchr( start, '}', '\"'); //ignore "text}"-parts
		if (start == NULL)
		{
			free(data);
			SDL_mutexP(spNetC4AStatusMutex);
			spNetC4AStatus = SP_C4A_ERROR;
			SDL_mutexV(spNetC4AStatusMutex);
			return 1;
		}
		//Creating substring:
		end[0] = 0;
		//Now we search in the substring
		//Search for the long name:
		char* pos = strstr( start, "\"longname\":");
		pos+=11;
		char longname[128];
		fill_between_paraphrases( pos, longname, 128);
		
		pos = strstr( start, "\"shortname\":");
		pos+=12;
		char shortname[128];
		fill_between_paraphrases( pos, shortname, 128);
		
		pos = strstr( start, "\"score\":");
		pos+=8;
		int score = atoi(pos);
		
		pos = strstr( start, "\"time\":");
		pos+=7;
		Uint64 commitTime = (Uint64)(atof(pos)); //float becase of bigger numbers
		
		//Re"inserting" substring:
		end[0] = '}';
		found = strchr( end, '{' );
		
		//Adding the found stuff to the array:
		if (longname[0] == 0 || shortname[0] == 0)
			continue;
		if (scoreData->profile && (strcmp(scoreData->profile->longname,longname) != 0 || strcmp(scoreData->profile->shortname,shortname) != 0))
			continue;
		spNetC4AScorePointer new_score = (spNetC4AScorePointer)malloc(sizeof(spNetC4AScore));
		sprintf(new_score->longname,"%s",longname);
		sprintf(new_score->shortname,"%s",shortname);
		new_score->score = score;
		new_score->commitTime = commitTime;
		new_score->next = NULL;
		if (lastScore == NULL)
			(*(scoreData->score)) = new_score;
		else
			lastScore->next = new_score;
		lastScore = new_score;
	}	
	
	free(data);
	SDL_mutexP(spNetC4AStatusMutex);
	spNetC4AStatus = SP_C4A_OK;
	SDL_mutexV(spNetC4AStatusMutex);
	return 0;
}

PREFIX SDL_Thread* spNetC4AGetScore(spNetC4AScorePointer* scoreList,spNetC4AProfilePointer profile,char* game)
{
	(*scoreList) = NULL;
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	if (status == SP_C4A_OK || status == SP_C4A_ERROR)
	{
		//Starting a background thread, which does the fancy stuff
		getscorePointer data = (getscorePointer)malloc(sizeof(getscoreType));
		data->score = scoreList;
		data->profile = profile;
		data->year = 0;
		data->month = 0;
		sprintf(data->game,"%s",game);
		return SDL_CreateThread(c4a_getscore_thread,data);
	}
	return NULL;
}

PREFIX SDL_Thread* spNetC4AGetScoreOfMonth(spNetC4AScorePointer* scoreList,spNetC4AProfilePointer profile,char* game,int year,int month)
{
	(*scoreList) = NULL;
	if (month < 1 || month > 12)
		return NULL;
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	if (status == SP_C4A_OK || status == SP_C4A_ERROR)
	{
		//Starting a background thread, which does the fancy stuff
		getscorePointer data = (getscorePointer)malloc(sizeof(getscoreType));
		data->score = scoreList;
		data->profile = profile;
		data->year = year;
		data->month = month;
		sprintf(data->game,"%s",game);
		return SDL_CreateThread(c4a_getscore_thread,data);
	}
	return NULL;
}


int c4a_commit_thread(void* data)
{
	SDL_mutexP(spNetC4AStatusMutex);
	spNetC4AStatus = SP_C4A_ESTABLISHING;
	SDL_mutexV(spNetC4AStatusMutex);
	commitPointer commitData = (commitPointer)data;
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		free(data);
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	SDL_mutexP(spNetC4AStatusMutex);
	spNetC4AStatus = SP_C4A_PROGRESS;
	SDL_mutexV(spNetC4AStatusMutex);
	char commit_string[2048];
	sprintf(commit_string,"score=%i",commitData->score);
	int dataSize = strlen(commit_string);
	sprintf(commit_string,"PUT /plugtally_1/scoreonly/%s/%s/%s?score=%i HTTP/1.1\r\nHost: skeezix.wallednetworks.com:13001\r\nAccept: */*\r\nContent-Length: 0\r\nExpect: 100-continue\r\n",commitData->game,commitData->system,commitData->profile->prid,commitData->score);
	if (spNetSendHTTP(connection,commit_string) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	//printf("Did:\n%s",commit_string);
	spNetCloseTCP(connection);
	//Adding to scoreList ;)
	if (commitData->scoreList)
	{
		spNetC4AScorePointer new_score = (spNetC4AScorePointer)malloc(sizeof(spNetC4AScore));
		sprintf(new_score->longname,"%s",commitData->profile->longname);
		sprintf(new_score->shortname,"%s",commitData->profile->shortname);
		new_score->score = commitData->score;
		new_score->commitTime = time(NULL);
		new_score->next = (*(commitData->scoreList));
		(*(commitData->scoreList)) = new_score;
	}
	free(data);
	SDL_mutexP(spNetC4AStatusMutex);
	spNetC4AStatus = SP_C4A_OK;
	SDL_mutexV(spNetC4AStatusMutex);
	return 0;
}

int already_in_highscore(spNetC4AScorePointer scoreList,spNetC4AProfilePointer profile,int score)
{
	if (scoreList == NULL)
		return 0;
	while (scoreList)
	{
		if (strcmp(scoreList->longname,profile->longname) == 0 &&
		    strcmp(scoreList->shortname,profile->shortname) == 0 &&
		    scoreList->score == score)
			return 1;
		scoreList = scoreList->next;
	}
	return 0;
}

PREFIX SDL_Thread* spNetC4ACommitScore(spNetC4AProfilePointer profile,char* game,int score,spNetC4AScorePointer* scoreList)
{
	if (profile == NULL)
		return NULL;
	if (already_in_highscore(*scoreList,profile,score))
		return NULL;
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	if (status == SP_C4A_OK || status == SP_C4A_ERROR)
	{
		//Starting a background thread, which does the fancy stuff
		commitPointer data = (commitPointer)malloc(sizeof(commitType));
		data->score = score;
		data->profile = profile;
		data->scoreList = scoreList;
		sprintf(data->game,"%s",game);
		#ifdef GP2X
			sprintf(data->system,"gp2x");
		#elif defined(CAANOO)
			sprintf(data->system,"caanoo");
		#elif defined(WIZ)
			sprintf(data->system,"wiz");
		#elif defined(DINGUX)
				sprintf(data->system,"dingux");
		#elif defined(GCW)	
			sprintf(data->system,"gcw");
		#elif defined(PANDORA)	
			sprintf(data->system,"pandora");
		#elif defined(WIN32)
			sprintf(data->system,"win32");
		#else
			sprintf(data->system,"linux");
		#endif
		return SDL_CreateThread(c4a_commit_thread,data);
	}
	return NULL;
}

PREFIX int spNetC4AGetStatus()
{
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	return status;
}

PREFIX void spNetC4ADeleteScores(spNetC4AScorePointer* scoreList)
{
	if (scoreList == NULL)
		return;
	while (*scoreList)
	{
		spNetC4AScorePointer next = (*scoreList)->next;
		free(*scoreList);
		(*scoreList) = next;
	}
}
