 /* This file is part of sparrow3d.
  * Sparrow3d is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  * 
  * Sparrow3d is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with Foobar.  If not, see <http://www.gnu.org/licenses/>
  * 
  * For feedback and questions about my Files and Projects please mail me,
  * Alexander Matthes (Ziz) , zizsdl_at_googlemail.com */

#include "sparrowNet.h"
#include <stdio.h>
#include <errno.h>

//This is a copy of spReadOneLine sparrowFile. However, I don't want the
//extra dependency of libSparrow3d or linking sparrowFile twice.
int internal_spNet_spReadOneLine( SDL_RWops* file , char* buffer, int buffer_len)
{
	int pos = 0;
	buffer[pos] = 0;
	while (pos < buffer_len)
	{
		if (SDL_RWread( file, &(buffer[pos]), 1, 1 ) <= 0)
			return 1; //EOF
		if ( buffer[pos] == '\n' )
			break;
		if (buffer[pos] != '\r') //fucking windows line break
			pos++;
	}
	buffer[pos] = 0;
	return 0; //not EOF
}

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
	return result;
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
	return result;
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
	int res=spNetReceiveHTTP(tcpData->connection,(char*)tcpData->data,tcpData->length);
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
					return (SDL_Thread*)(-1);
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
void *spNetC4ADataPointer = NULL;
int spNetC4ATimeOut = 0;
SDL_Thread* spNetC4AThread = NULL;
int spNetC4AResult = 0;
int spNetC4AThreadStatus = 0;

typedef struct getgameStruct *getgamePointer;
typedef struct getgameStruct {
	spNetC4AGamePointer* game;
} getgameType;

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

int spNetC4AUberThread(int ( *function )( void* data ))
{
	int startTime = SDL_GetTicks();
	SDL_Thread* thread = SDL_CreateThread(function,spNetC4ADataPointer);
	while (1)
	{
	#ifdef REALGP2X
		//TODO: Implement!
		SDL_Delay(1);
	#elif defined WIN32
		SDL_Delay(1);	
	#else
		usleep(100);
	#endif
		int newTime = SDL_GetTicks();
		int diff = newTime - startTime;
		startTime = newTime;
		spNetC4ATimeOut -= diff;
		SDL_mutexP(spNetC4AStatusMutex);
		int status = spNetC4AStatus;
		SDL_mutexV(spNetC4AStatusMutex);
		if (status == SP_C4A_CANCELED || spNetC4ATimeOut <= 0)
		{
			SDL_KillThread(thread);
			free(spNetC4ADataPointer);
			spNetC4ADataPointer = NULL;
			spNetC4AResult = 1;
			SDL_mutexP(spNetC4AStatusMutex);
			if (spNetC4ATimeOut <= 0)
				spNetC4AStatus = SP_C4A_TIMEOUT;
			spNetC4AThreadStatus = 0;
			SDL_mutexV(spNetC4AStatusMutex);
			return spNetC4AResult;
		}
		if (status <= 0) //finished somehow
		{
			SDL_WaitThread(thread,&spNetC4AResult);
			SDL_mutexP(spNetC4AStatusMutex);
			spNetC4AThreadStatus = 0;
			SDL_mutexV(spNetC4AStatusMutex);
			return spNetC4AResult;
		}
	}
}

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

void internal_CreateDirectoryChain( const char* directories)
{
	//Creating copy:
	int len = strlen(directories)+1;
	#ifdef __GNUC__
		char directoriesCopy[len];
	#else
		char* directoriesCopy = (char*)malloc( len * sizeof(char) );
	#endif
	memcpy(directoriesCopy,directories,len);
	//Splitting in subdirectories
	char* subString = directoriesCopy;
	char* endOfString = strchr(subString,'/');
	if (endOfString == NULL)
		endOfString = strchr(subString,0);
	while (endOfString)
	{
		char oldChar = endOfString[0];
		endOfString[0] = 0;
		#ifdef WIN32

			if (CreateDirectory(directoriesCopy,NULL))
			{}
			else
			if (GetLastError() != ERROR_ALREADY_EXISTS)
				break;
		#else
			int error = mkdir(directoriesCopy,0777);
			if (errno == 0 || errno == EEXIST || errno == ENOENT) //thats okay :)
			{}
			else //not okay
				break;
		#endif
		endOfString[0] = oldChar;
		if (oldChar == 0)
			break;
		subString = &(endOfString[1]);
		endOfString = strchr(subString,'/');
		if (endOfString == NULL)
			endOfString = strchr(subString,0);
	}
	#ifndef __GNUC__
		free(directoriesCopy);
	#endif
}

#ifdef PANDORA
	#define PROFILE_FILENAME_MAKRO char *filename = pnd_locate_filename ( "/media/*/pandora/appdata/c4a-mame/:.", "c4a-prof" );
#elif defined GCW || (defined X86CPU && !defined WIN32)
	#define PROFILE_FILENAME_MAKRO char filename[256]; \
		sprintf(filename,"%s/.config/compo4all",getenv("HOME"));\
		internal_CreateDirectoryChain(filename);\
		sprintf(filename,"%s/.config/compo4all/c4a-prof",getenv("HOME"));
#else
	#define PROFILE_FILENAME_MAKRO char filename[256] = "./c4a-prof";
#endif

PREFIX spNetC4AProfilePointer spNetC4AGetProfile()
{
	spNetC4AProfilePointer profile = NULL;
	PROFILE_FILENAME_MAKRO
	//Parsing the file
	SDL_RWops *file=SDL_RWFromFile(filename,"rb");
	if (file == NULL)
		return NULL;
	profile = (spNetC4AProfilePointer)malloc(sizeof(spNetC4AProfile));
	char buffer[2048];
	internal_spNet_spReadOneLine(file,buffer,2048);
	internal_spNet_spReadOneLine(file,buffer,2048);
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

int c4a_getgame_thread(void* data)
{
	spNetC4AGamePointer* gameList = ((getgamePointer)data)->game;
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	if (ip.address.ipv4 == SP_INVALID_IP)
	{
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}	
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	char get_string[512] = "GET /curgamelist_1\n\n";
	if (spNetSendHTTP(connection,get_string) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		spNetC4ADataPointer = NULL;
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
		spNetC4ADataPointer = NULL;
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
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	//Reading game by game
	//Searching the starting {
	while (found)
	{
		char* start = strchr( found, '{' );
		if (start == NULL)
		{
			free(data);
			spNetC4ADataPointer = NULL;
			SDL_mutexP(spNetC4AStatusMutex);
			spNetC4AStatus = SP_C4A_ERROR;
			SDL_mutexV(spNetC4AStatusMutex);
			return 1;
		}
		char* end = my_strchr( start, '}', '\"'); //ignore "text}"-parts
		if (start == NULL)
		{
			free(data);
			spNetC4ADataPointer = NULL;
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
		char longname[256];
		fill_between_paraphrases( pos, longname, 128);
		
		pos = strstr( start, "\"gamename\":");
		pos+=11;
		char shortname[256];
		fill_between_paraphrases( pos, shortname, 128);
		
		pos = strstr( start, "\"genre\":");
		pos+=8;
		char genre[256];
		fill_between_paraphrases( pos, genre, 128);
		
		pos = strstr( start, "\"field\":");
		pos+=8;
		char field[256];
		fill_between_paraphrases( pos, field, 128);
		
		pos = strstr( start, "\"status\":");
		pos+=9;
		char status[256];
		fill_between_paraphrases( pos, status, 128);
		
		//Re"inserting" substring:
		end[0] = '}';
		found = strchr( end, '{' );
		
		//Adding the found stuff to the array:
		spNetC4AGamePointer new_game = (spNetC4AGamePointer)malloc(sizeof(spNetC4AGame));
		sprintf(new_game->longname,"%s",longname);
		sprintf(new_game->shortname,"%s",shortname);
		sprintf(new_game->genre,"%s",genre);
		if (strcmp(status,"available") == 0)
			new_game->status = 1;
		else
		if (strcmp(status,"active") == 0)
			new_game->status = 2;
		else
			new_game->status = 0;
		if (strcmp(field,"arcade") == 0)
			new_game->field = 1;
		else
		if (strcmp(field,"indie") == 0)
			new_game->field = 0;
		else
			new_game->field = -1;
		
		//sorted insert
		//Searching the next and before element:
		spNetC4AGamePointer before = NULL;
		spNetC4AGamePointer next = *gameList;
		while (next)
		{
			if (strcmp(new_game->longname,next->longname) < 0)
				break;
			before = next;
			next = next->next;
		}
		if (before == NULL) //new first element!
		{
			new_game->next = next;
			(*gameList) = new_game;
		}
		else
		{
			before->next = new_game;
			new_game->next = next;
		}
	}	
	
	free(data);
	spNetC4ADataPointer = NULL;
	SDL_mutexP(spNetC4AStatusMutex);
	spNetC4AStatus = SP_C4A_OK;
	SDL_mutexV(spNetC4AStatusMutex);
	return 0;
}

PREFIX int spNetC4AGetGame(spNetC4AGamePointer* gameList,int timeOut)
{
	(*gameList) = NULL;
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	if (status == SP_C4A_OK || status == SP_C4A_ERROR)
	{
		spNetC4AStatus = SP_C4A_PROGRESS;
		//Starting a background thread, which does the fancy stuff
		getgamePointer data = (getgamePointer)malloc(sizeof(getgameType));
		data->game = gameList;
		spNetC4ADataPointer = data;
		spNetC4ATimeOut = timeOut;
		spNetC4AThreadStatus = 1;
		#ifdef _MSC_VER
			spNetC4AThread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,c4a_getgame_thread);
		#else
			spNetC4AThread = SDL_CreateThread(spNetC4AUberThread,c4a_getgame_thread);
		#endif
		return 0;
	}
	return 1;	
}

int c4a_getscore_thread(void* data)
{
	getscorePointer scoreData = (getscorePointer)data;
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	if (ip.address.ipv4 == SP_INVALID_IP)
	{
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}	
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	char get_string[512];
	if (scoreData->year && scoreData->month)
		sprintf(get_string,"GET /json_1/%s/%i%02i/\n\n",scoreData->game,scoreData->year,scoreData->month);
	else
		sprintf(get_string,"GET /json_1/%s/all\n\n",scoreData->game);
	if (spNetSendHTTP(connection,get_string) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		spNetC4ADataPointer = NULL;
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
		spNetC4ADataPointer = NULL;
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
		spNetC4ADataPointer = NULL;
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
			spNetC4ADataPointer = NULL;
			SDL_mutexP(spNetC4AStatusMutex);
			spNetC4AStatus = SP_C4A_ERROR;
			SDL_mutexV(spNetC4AStatusMutex);
			return 1;
		}
		char* end = my_strchr( start, '}', '\"'); //ignore "text}"-parts
		if (start == NULL)
		{
			free(data);
			spNetC4ADataPointer = NULL;
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
	spNetC4ADataPointer = NULL;
	SDL_mutexP(spNetC4AStatusMutex);
	spNetC4AStatus = SP_C4A_OK;
	SDL_mutexV(spNetC4AStatusMutex);
	return 0;
}

PREFIX void spNetC4ADeleteGames(spNetC4AGamePointer* gameList)
{
	if (gameList == NULL)
		return;
	while (*gameList)
	{
		spNetC4AGamePointer next = (*gameList)->next;
		free(*gameList);
		(*gameList) = next;
	}
}

PREFIX int spNetC4AGetScore(spNetC4AScorePointer* scoreList,spNetC4AProfilePointer profile,char* game,int timeOut)
{
	(*scoreList) = NULL;
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	if (status == SP_C4A_OK || status == SP_C4A_ERROR)
	{
		spNetC4AStatus = SP_C4A_PROGRESS;
		//Starting a background thread, which does the fancy stuff
		getscorePointer data = (getscorePointer)malloc(sizeof(getscoreType));
		data->score = scoreList;
		data->profile = profile;
		data->year = 0;
		data->month = 0;
		sprintf(data->game,"%s",game);
		spNetC4ADataPointer = data;
		spNetC4ATimeOut = timeOut;
		spNetC4AThreadStatus = 1;
		#ifdef _MSC_VER
			spNetC4AThread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,c4a_getscore_thread);
		#else
			spNetC4AThread = SDL_CreateThread(spNetC4AUberThread,c4a_getscore_thread);
		#endif
		return 0;
	}
	return 1;
}

PREFIX int spNetC4AGetScoreOfMonth(spNetC4AScorePointer* scoreList,spNetC4AProfilePointer profile,char* game,int year,int month,int timeOut)
{
	(*scoreList) = NULL;
	if (month < 1 || month > 12)
		return 1;
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	if (status == SP_C4A_OK || status == SP_C4A_ERROR)
	{
		spNetC4AStatus = SP_C4A_PROGRESS;
		//Starting a background thread, which does the fancy stuff
		getscorePointer data = (getscorePointer)malloc(sizeof(getscoreType));
		data->score = scoreList;
		data->profile = profile;
		data->year = year;
		data->month = month;
		sprintf(data->game,"%s",game);
		spNetC4ADataPointer = data;
		spNetC4ATimeOut = timeOut;
		spNetC4AThreadStatus = 1;
		#ifdef _MSC_VER
			spNetC4AThread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,c4a_getscore_thread);
		#else
			spNetC4AThread = SDL_CreateThread(spNetC4AUberThread,c4a_getscore_thread);
		#endif
		return 0;
	}
	return 1;
}


int c4a_commit_thread(void* data)
{
	commitPointer commitData = (commitPointer)data;
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	if (ip.address.ipv4 == SP_INVALID_IP)
	{
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}	
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	char commit_string[2048];
	sprintf(commit_string,"score=%i",commitData->score);
	int dataSize = strlen(commit_string);
	sprintf(commit_string,"PUT /plugtally_1/scoreonly/%s/%s/%s?score=%i HTTP/1.1\r\nHost: skeezix.wallednetworks.com:13001\r\nAccept: */*\r\nContent-Length: 0\r\nExpect: 100-continue\r\n",commitData->game,commitData->system,commitData->profile->prid,commitData->score);
	if (spNetSendHTTP(connection,commit_string) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		spNetC4ADataPointer = NULL;
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
	spNetC4ADataPointer = NULL;
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

PREFIX int spNetC4ACommitScore(spNetC4AProfilePointer profile,char* game,int score,spNetC4AScorePointer* scoreList,int timeOut)
{
	if (profile == NULL)
		return 1;
	if (scoreList && already_in_highscore(*scoreList,profile,score))
		return 1;
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	if (status == SP_C4A_OK || status == SP_C4A_ERROR)
	{
		spNetC4AStatus = SP_C4A_PROGRESS;
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
		spNetC4ADataPointer = data;
		spNetC4ATimeOut = timeOut;
		spNetC4AThreadStatus = 1;
		#ifdef _MSC_VER
			spNetC4AThread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,c4a_commit_thread);
		#else
			spNetC4AThread = SDL_CreateThread(spNetC4AUberThread,c4a_commit_thread);
		#endif
		return 0;
	}
	return 1;
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

void fill_with_random_hex(char* buffer,int count)
{
	int i;
	for (i = 0; i < count; i++)
	{
		int c = rand()%16;
		char cha;
		if (c < 10)
			cha = c+'0';
		else
			cha = c-10+'a';
		buffer[i] = cha;
	}
}

typedef struct createStruct *createPointer;
typedef struct createStruct {
	spNetC4AProfilePointer* profile;
	char longname[256];
	char shortname[256];
	char password[256];
	char email[256];
	int deleteFile;
} createType;

int c4a_create_thread(void* data)
{
	createPointer createData = (createPointer)data;
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	if (ip.address.ipv4 == SP_INVALID_IP)
	{
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}	
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	char create_string[2048];
	char buffer[2048];
	char prid[37] = "";
	//generating a new, random prid:
	fill_with_random_hex(prid,8);
	prid[ 8]='-';
	fill_with_random_hex(&(prid[ 9]),4);
	prid[13]='-';
	fill_with_random_hex(&(prid[14]),4);
	prid[18]='-';
	fill_with_random_hex(&(prid[19]),4);
	prid[23]='-';
	fill_with_random_hex(&(prid[24]),12);
	prid[36]=0;
	sprintf(create_string,"{\"email\": \"%s\", \"shortname\": \"%s\", \"password\": \"%s\", \"prid\": \"%s\", \"longname\": \"%s\"}",createData->email,createData->shortname,createData->password,prid,createData->longname);
	sprintf(buffer,"PUT /setprofile_1 HTTP/1.1\r\nUser-Agent: sparrowNet/1.0\r\nHost: %i.%i.%i.%i:13001\r\nAccept: */*\r\nContent-Length: %i\r\nExpect: 100-continue\r\n\r\n",ip.address.ipv4_bytes[0],ip.address.ipv4_bytes[1],ip.address.ipv4_bytes[2],ip.address.ipv4_bytes[3],strlen(create_string));
	if (spNetSendTCP(connection,buffer,strlen(buffer)) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	if (spNetSendTCP(connection,create_string,strlen(create_string)) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	spNetCloseTCP(connection);
	PROFILE_FILENAME_MAKRO
	SDL_RWops *file=SDL_RWFromFile(filename,"wb");
	SDL_RWwrite(file,prid,36,1);
	char c = '\n';
	SDL_RWwrite(file,&c,1,1);
	SDL_RWwrite(file,create_string,strlen(create_string),1);
	SDL_RWclose(file);
	(*(createData->profile)) = (spNetC4AProfilePointer)malloc(sizeof(spNetC4AProfile));
	sprintf((*(createData->profile))->longname,"%s",createData->longname);
	sprintf((*(createData->profile))->shortname,"%s",createData->shortname);
	sprintf((*(createData->profile))->password,"%s",createData->password);
	sprintf((*(createData->profile))->email,"%s",createData->email);
	sprintf((*(createData->profile))->prid,"%s",prid);
	free(data);
	spNetC4ADataPointer = NULL;
	SDL_mutexP(spNetC4AStatusMutex);
	spNetC4AStatus = SP_C4A_OK;
	SDL_mutexV(spNetC4AStatusMutex);
	return 0;	
}

PREFIX int spNetC4ACreateProfile(spNetC4AProfilePointer* profile, char* longname,char* shortname,char* password,char* email,int timeOut)
{
	if (profile == NULL)
		return 1;
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	if (status == SP_C4A_OK || status == SP_C4A_ERROR)
	{
		spNetC4AStatus = SP_C4A_PROGRESS;
		//Starting a background thread, which does the fancy stuff
		createPointer data = (createPointer)malloc(sizeof(createType));
		data->profile = profile;
		sprintf(data->longname,"%s",longname);
		sprintf(data->shortname,"%s",shortname);
		sprintf(data->password,"%s",password);
		sprintf(data->email,"%s",email);
		spNetC4ADataPointer = data;
		spNetC4ATimeOut = timeOut;
		spNetC4AThreadStatus = 1;
		#ifdef _MSC_VER
			spNetC4AThread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,c4a_create_thread);
		#else
			spNetC4AThread = SDL_CreateThread(spNetC4AUberThread,c4a_create_thread);
		#endif
		return 0;
	}
	return 1;
}

int c4a_delete_thread(void* data)
{
	createPointer createData = (createPointer)data;
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	if (ip.address.ipv4 == SP_INVALID_IP)
	{
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}	
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	char create_string[2048];
	char buffer[2048];
	sprintf(create_string,"{\"email\": \"%s\", \"shortname\": \"%s\", \"password\": \"%s\", \"prid\": \"%s\", \"longname\": \"%s\"}",(*(createData->profile))->email,(*(createData->profile))->shortname,(*(createData->profile))->password,(*(createData->profile))->prid,(*(createData->profile))->longname);
	sprintf(buffer,"PUT /delprofile_1 HTTP/1.1\r\nUser-Agent: sparrowNet/1.0\r\nHost: %i.%i.%i.%i:13001\r\nAccept: */*\r\nContent-Length: %i\r\nExpect: 100-continue\r\n\r\n",ip.address.ipv4_bytes[0],ip.address.ipv4_bytes[1],ip.address.ipv4_bytes[2],ip.address.ipv4_bytes[3],strlen(create_string));
	if (spNetSendTCP(connection,buffer,strlen(buffer)) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	if (spNetSendTCP(connection,create_string,strlen(create_string)) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	spNetCloseTCP(connection);
	(*(createData->profile)) = NULL;	
	if (createData->deleteFile)
		spNetC4ADeleteProfileFile();
	free(data);
	spNetC4ADataPointer = NULL;
	SDL_mutexP(spNetC4AStatusMutex);
	spNetC4AStatus = SP_C4A_OK;
	SDL_mutexV(spNetC4AStatusMutex);
	return 0;
}

PREFIX int spNetC4ADeleteAccount(spNetC4AProfilePointer* profile,int deleteFile,int timeOut)
{
	if (profile == NULL)
		return 1;
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	if (status == SP_C4A_OK || status == SP_C4A_ERROR)
	{
		spNetC4AStatus = SP_C4A_PROGRESS;
		//Starting a background thread, which does the fancy stuff
		createPointer data = (createPointer)malloc(sizeof(createType));
		data->profile = profile;
		data->deleteFile = deleteFile;
		spNetC4ADataPointer = data;
		spNetC4ATimeOut = timeOut;
		spNetC4AThreadStatus = 1;
		#ifdef _MSC_VER
			spNetC4AThread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,c4a_delete_thread);
		#else
			spNetC4AThread = SDL_CreateThread(spNetC4AUberThread,c4a_delete_thread);
		#endif
		return 0;
	}
	return 1;
}

PREFIX void spNetC4ADeleteProfileFile()
{
	PROFILE_FILENAME_MAKRO
//Copied from spRemoveFile to avoid dependencies
#ifdef WIN32
	DeleteFile(filename);
#else
	remove(filename);
#endif	
}

int c4a_edit_thread(void* data)
{
	createPointer createData = (createPointer)data;
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	if (ip.address.ipv4 == SP_INVALID_IP)
	{
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}	
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	char create_string[2048];
	char buffer[2048];
	sprintf(create_string,"{\"email\": \"%s\", \"shortname\": \"%s\", \"password\": \"%s\", \"prid\": \"%s\", \"longname\": \"%s\"}",createData->email,createData->shortname,createData->password,(*(createData->profile))->prid,createData->longname);
	sprintf(buffer,"PUT /setprofile_1 HTTP/1.1\r\nUser-Agent: sparrowNet/1.0\r\nHost: %i.%i.%i.%i:13001\r\nAccept: */*\r\nContent-Length: %i\r\nExpect: 100-continue\r\n\r\n",ip.address.ipv4_bytes[0],ip.address.ipv4_bytes[1],ip.address.ipv4_bytes[2],ip.address.ipv4_bytes[3],strlen(create_string));
	if (spNetSendTCP(connection,buffer,strlen(buffer)) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	if (spNetSendTCP(connection,create_string,strlen(create_string)) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		spNetC4ADataPointer = NULL;
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	spNetCloseTCP(connection);
	PROFILE_FILENAME_MAKRO
	SDL_RWops *file=SDL_RWFromFile(filename,"wb");
	SDL_RWwrite(file,(*(createData->profile))->prid,strlen((*(createData->profile))->prid),1);
	char c = '\n';
	SDL_RWwrite(file,&c,1,1);
	SDL_RWwrite(file,create_string,strlen(create_string),1);
	SDL_RWclose(file);
	sprintf((*(createData->profile))->longname,"%s",createData->longname);
	sprintf((*(createData->profile))->shortname,"%s",createData->shortname);
	sprintf((*(createData->profile))->password,"%s",createData->password);
	sprintf((*(createData->profile))->email,"%s",createData->email);
	free(data);
	spNetC4ADataPointer = NULL;
	SDL_mutexP(spNetC4AStatusMutex);
	spNetC4AStatus = SP_C4A_OK;
	SDL_mutexV(spNetC4AStatusMutex);
	return 0;	
}

PREFIX int spNetC4AEditProfile(spNetC4AProfilePointer* profile,char* longname,char* shortname,char* password,char* email,int timeOut)
{
	if (profile == NULL)
		return 1;
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	if (status == SP_C4A_OK || status == SP_C4A_ERROR)
	{
		spNetC4AStatus = SP_C4A_PROGRESS;
		//Starting a background thread, which does the fancy stuff
		createPointer data = (createPointer)malloc(sizeof(createType));
		data->profile = profile;
		sprintf(data->longname,"%s",longname);
		sprintf(data->shortname,"%s",shortname);
		sprintf(data->password,"%s",password);
		sprintf(data->email,"%s",email);
		spNetC4ADataPointer = data;
		spNetC4ATimeOut = timeOut;
		spNetC4AThreadStatus = 1;
		#ifdef _MSC_VER
			spNetC4AThread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,c4a_edit_thread);
		#else
			spNetC4AThread = SDL_CreateThread(spNetC4AUberThread,c4a_edit_thread);
		#endif
		return 0;
	}
	return 1;
}

PREFIX int spNetC4AGetStatus()
{
	SDL_mutexP(spNetC4AStatusMutex);
	if (spNetC4AThreadStatus)
	{
		SDL_mutexV(spNetC4AStatusMutex);
		return SP_C4A_PROGRESS;
	}
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	return status;
}

PREFIX void spNetC4ACancelTask()
{
	SDL_mutexP(spNetC4AStatusMutex);
	if (spNetC4AStatus > 0)
	{
		spNetC4AStatus = SP_C4A_CANCELED;
		SDL_mutexV(spNetC4AStatusMutex);
		SDL_WaitThread(spNetC4AThread,NULL);
	}
	else
		SDL_mutexV(spNetC4AStatusMutex);
}

PREFIX int spNetC4AGetTaskResult()
{
	return spNetC4AResult;
}

PREFIX int spNetC4AGetTimeOut()
{
	return spNetC4ATimeOut;
}
