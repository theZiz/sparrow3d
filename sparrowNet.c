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
#include <math.h>
#ifdef __GNUC__
	#include <sys/stat.h>
#endif
#ifndef _WIN32
	#include <unistd.h>
#endif

//#define DEBUG_MESSAGES

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

#define DIGIT_CHECK(v) \
	((v) == '0' || (v) == '1' || \
	 (v) == '2' || (v) == '3' || \
	 (v) == '4' || (v) == '5' || \
	 (v) == '6' || (v) == '7' || \
	 (v) == '8' || (v) == '9')

PREFIX double internal_spNet_spAtoFloat( char* buffer )
{
	int i = 0;
	while (buffer[i] == ' ' && buffer[i] != 0)
		i++;
	if (buffer[i] == 0)
		return 0.0f;
	double result = 0.0f;
	double sign = 1.0f;
	Sint64 left = 0;
	Sint64 middle = 0;
	Sint64 divisor = 1;
	Sint64 right = 0;
	//sign
	if (buffer[i] == '-')
	{
		sign = -1.0f;
		i++;
	}
	//left part
	while ( DIGIT_CHECK(buffer[i]) )
	{
		left *= 10;
		int digit = (int)buffer[i] - (int)'0';
		left += digit;
		i++;
	}
	//middle part
	if (buffer[i] == '.')
	{
		i++;
		while ( DIGIT_CHECK(buffer[i]) )
		{
			middle *= 10;
			int digit = (int)buffer[i] - (int)'0';
			middle += digit;
			divisor *= 10;
			i++;
		}
	}
	//right part
	if (buffer[i] == 'e' || buffer[i] == 'E')
	{
		i++;
		while ( DIGIT_CHECK(buffer[i]) )
		{
			right *= 10;
			int digit = (int)buffer[i] - (int)'0';
			right += digit;
			i++;
		}
	}
	if (right)
		return sign*((double)left + (double)middle/(double)divisor)*pow(10.0f,(double)right);
	else
		return sign*((double)left + (double)middle/(double)divisor);
}

spNetC4ATaskPointer spGlobalC4ATask = NULL;
SDL_mutex* spCacheMutex = NULL;

spNetC4ATaskPointer createNewC4ATask( void )
{
	spNetC4ATaskPointer task = (spNetC4ATaskPointer)malloc(sizeof(spNetC4ATask));
	task->statusMutex = SDL_CreateMutex();
	task->status = 0;
	task->dataPointer = NULL;
	task->timeOut = 0;
	task->thread = NULL;
	task->result = 0;
	task->threadStatus = 0;
	task->message = 0;
	return task;
}

void spNetC4ADeleteTask(spNetC4ATaskPointer task)
{
	SDL_DestroyMutex(task->statusMutex);
	free(task);
}

PREFIX void spInitNet( void )
{
	if(SDLNet_Init()==-1)
		printf("SDLNet_Init: %s\n", SDLNet_GetError());
	spGlobalC4ATask = createNewC4ATask();
	spCacheMutex = SDL_CreateMutex();
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

PREFIX int spNetReceiveFinished(SDL_Thread* thread)
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
				int res = mom->result;
				free(mom);
				return res;
			}
			SDL_mutexV(mom->mutex);
			return 0;
		}
		before = mom;
		mom = mom->next;
	}
	return -1;
}

PREFIX void spNetCloseTCP(spNetTCPConnection connection)
{
	SDLNet_TCP_Close(connection);
}

PREFIX void spQuitNet( void )
{
	spNetC4ADeleteTask(spGlobalC4ATask);
	spGlobalC4ATask = NULL;
	SDL_mutexP(spCacheMutex);
	SDL_DestroyMutex(spCacheMutex);
	SDLNet_Quit();
}

#ifdef PANDORA
	#include "pnd_locate.h"
#endif

typedef struct getgenericStruct *getgenericPointer;
typedef struct getgenericStruct {
	spNetC4ATaskPointer task;
	int ( *function )( void* data );
} getgenericType;

typedef struct getgameStruct *getgamePointer;
typedef struct getgameStruct {
	spNetC4ATaskPointer task;
	int ( *function )( void* data );
	spNetC4AGamePointer* game;
} getgameType;

typedef struct getscoreStruct *getscorePointer;
typedef struct getscoreStruct {
	spNetC4ATaskPointer task;
	int ( *function )( void* data );
	spNetC4AScorePointer* score;
	spNetC4AProfilePointer profile;
	int year;
	int month;
	char game[256];
} getscoreType;

typedef struct commitStruct *commitPointer;
typedef struct commitStruct {
	spNetC4ATaskPointer task;
	int ( *function )( void* data );
	spNetC4AProfilePointer profile;
	char game[256];
	int score;
	char system[256];
	spNetC4AScorePointer* scoreList;
} commitType;

typedef struct createStruct *createPointer;
typedef struct createStruct {
	spNetC4ATaskPointer task;
	int ( *function )( void* data );
	spNetC4AProfilePointer* profile;
	char longname[256];
	char shortname[256];
	char password[256];
	char email[256];
	int deleteFile;
} createType;

int spNetC4ACaching = 0;
char spCacheFilename[256] = "";

//This is usefull for debugging without threading influences:
/*#define SDL_CreateThread SDL_CreateThreadWithoutThreading
SDL_Thread* SDL_CreateThreadWithoutThreading(int (*fn)(void *),void* data)
{
	fn(data);
	return NULL;
}*/


typedef struct cacheStruct *cachePointer;
typedef struct cacheStruct {
	char game[256];
	char system[256];
	char prid[256];
	int score;
	cachePointer next;
} cacheType;

cachePointer read_cache( void )
{
	SDL_RWops *file = SDL_RWFromFile(spCacheFilename, "rb");
	if (file)
	{
		char game[256];
		char system[256];
		char prid[256];
		int score;
		cachePointer cache = NULL;
		cachePointer last = NULL;
		while (1)
		{
			if (SDL_RWread(file,game,256,1) <= 0)
				break;
			if (SDL_RWread(file,system,256,1) <= 0)
				break;
			if (SDL_RWread(file,prid,256,1) <= 0)
				break;
			if (SDL_RWread(file,&score,sizeof(int),1) <= 0)
				break;
			cachePointer new_cache = (cachePointer)malloc(sizeof(cacheType));
			memcpy(new_cache->game,game,256);
			memcpy(new_cache->system,system,256);
			memcpy(new_cache->prid,prid,256);
			new_cache->score = score;
			new_cache->next = NULL;
			if (last)
				last->next = new_cache;
			else
				cache = new_cache;
			last = new_cache;
		}
		SDL_RWclose(file);
		return cache;
	}
	return NULL;
}

void write_to_cache(char* game,char* system,char* prid,int score,int lock)
{
	if (lock)
		SDL_mutexP(spCacheMutex);
	cachePointer cache = NULL;
	if (spNetC4ACaching != 1)
		cache = read_cache();
	//Searching one of game
	cachePointer mom = cache;
	int nr = 0;
	while (mom)
	{
		if (strcmp(mom->game,game) == 0)
			break;
		nr++;
		mom = mom->next;
	}
	if (mom)
	{
		if  ((spNetC4ACaching == 2 && mom->score < score) ||
			(spNetC4ACaching == 3 && mom->score > score))
		{
			//Seek and rewrite
			SDL_RWops *file = SDL_RWFromFile(spCacheFilename, "r+b");
			SDL_RWseek(file,nr*(256*3+sizeof(int))+256*3,SEEK_SET);
			SDL_RWwrite(file,&score,sizeof(int),1);
			SDL_RWclose(file);
		}
	}
	else
	{
		SDL_RWops *file = SDL_RWFromFile(spCacheFilename, "ab");
		SDL_RWwrite(file,game,256,1);
		SDL_RWwrite(file,system,256,1);
		SDL_RWwrite(file,prid,256,1);
		SDL_RWwrite(file,&score,sizeof(int),1);
		SDL_RWclose(file);
	}
	while (cache)
	{
		mom = cache->next;
		free(cache);
		cache = mom;
	}
	if (lock)
		SDL_mutexV(spCacheMutex);
}

int spNetC4AUberThread(getgenericPointer data)
{
	int startTime = SDL_GetTicks();
	SDL_Thread* thread = SDL_CreateThread(data->function,data);
	while (1)
	{
	#ifdef REALGP2X
		//TODO: Implement!
		SDL_Delay(100);
	#elif defined _WIN32
		SDL_Delay(100);
	#else
		usleep(100000);
	#endif
		int newTime = SDL_GetTicks();
		int diff = newTime - startTime;
		startTime = newTime;
		data->task->timeOut -= diff;
		SDL_mutexP(data->task->statusMutex);
		int status = data->task->status;
		SDL_mutexV(data->task->statusMutex);
		//only 1 second left, lets send a message
		if (data->task->message == 0 && (status == SP_C4A_CANCELED || data->task->timeOut <= 1000))
			data->task->message = 1;
		//time is over. If the message is reset we assume the thread will finish, otherwise...
		if (data->task->message == 1 && (status == SP_C4A_CANCELED || data->task->timeOut <= 0))
		{
			//Waiting for the write cache mutex ANYWAY.
			SDL_mutexP(spCacheMutex);
			SDL_KillThread(thread);
			SDL_mutexV(spCacheMutex);
			data->task->result = 1;
			SDL_mutexP(data->task->statusMutex);
			if (data->task->timeOut <= 0)
				data->task->status = SP_C4A_TIMEOUT;
			data->task->threadStatus = 0;
			SDL_mutexV(data->task->statusMutex);
			int result = data->task->result;
			data->task->dataPointer = NULL;
			free(data);
			return result;
		}
		if (status <= 0) //finished somehow
		{
			SDL_WaitThread(thread,&(data->task->result));
			SDL_mutexP(data->task->statusMutex);
			data->task->threadStatus = 0;
			SDL_mutexV(data->task->statusMutex);
			int result = data->task->result;
			data->task->dataPointer = NULL;
			free(data);
			return result;
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
		#ifdef _WIN32

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
	#define PROFILE_FILENAME_MAKRO char *locate_filename = pnd_locate_filename ( "/media/*/pandora/appdata/c4a-mame/:.", "c4a-prof" ); \
		char filename[256] = "./c4a-prof"; \
		if (locate_filename) \
			sprintf(filename,"%s",locate_filename); \
		else \
		{ \
			locate_filename = pnd_locate_filename ( "/media/*/pandora/:.", "appdata" ); \
			if (locate_filename) \
			{ \
				sprintf(filename,"%s/c4a-mame",locate_filename); \
				internal_CreateDirectoryChain(filename); \
				sprintf(filename,"%s/c4a-mame/c4a-prof",locate_filename); \
			} \
		}
#elif defined GCW || (defined DESKTOP && !defined _WIN32)
	#define PROFILE_FILENAME_MAKRO char filename[256]; \
		sprintf(filename,"%s/.config/compo4all",getenv("HOME"));\
		internal_CreateDirectoryChain(filename);\
		sprintf(filename,"%s/.config/compo4all/c4a-prof",getenv("HOME"));
#else
	#define PROFILE_FILENAME_MAKRO char filename[256] = "./c4a-prof";
#endif

PREFIX void spNetC4ASetCaching(int value)
{
	spNetC4ACaching = value;
}

void set_cache_filename( void )
{
	PROFILE_FILENAME_MAKRO
	sprintf(spCacheFilename,"%s",filename);
	sprintf(&spCacheFilename[strlen(spCacheFilename)-4],"cache");
}

PREFIX spNetC4AProfilePointer spNetC4AGetProfile( void )
{
	set_cache_filename();
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
	getgamePointer gameData = ((getgamePointer)data);
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	if (ip.address.ipv4 == SP_INVALID_IP)
	{
		SDL_mutexP(gameData->task->statusMutex);
		gameData->task->status = SP_C4A_ERROR;
		SDL_mutexV(gameData->task->statusMutex);
		return 1;
	}
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		SDL_mutexP(gameData->task->statusMutex);
		gameData->task->status = SP_C4A_ERROR;
		SDL_mutexV(gameData->task->statusMutex);
		return 1;
	}
	char get_string[512] = "GET /curgamelist_1\n\n";
	if (spNetSendHTTP(connection,get_string) == 0)
	{
		spNetCloseTCP(connection);
		SDL_mutexP(gameData->task->statusMutex);
		gameData->task->status = SP_C4A_ERROR;
		SDL_mutexV(gameData->task->statusMutex);
		return 1;
	}
	//skeezix saves the top500. So 100 byte should be enough...
	//Haha. NOT! minislug had 50950 with a top 500...
	char buffer[100001];
	int length;
	if ((length = spNetReceiveHTTP(connection,buffer,100000)) == 0)
	{
		spNetCloseTCP(connection);
		SDL_mutexP(gameData->task->statusMutex);
		gameData->task->status = SP_C4A_ERROR;
		SDL_mutexV(gameData->task->statusMutex);
		return 1;
	}
	buffer[length] = 0;
	spNetCloseTCP(connection);
	//Searching the first [
	char* found = strchr( buffer, '[' );
	if (found == NULL)
	{
		SDL_mutexP(gameData->task->statusMutex);
		gameData->task->status = SP_C4A_ERROR;
		SDL_mutexV(gameData->task->statusMutex);
		return 1;
	}
	//Reading game by game
	//Searching the starting {
	while (found)
	{
		char* start = strchr( found, '{' );
		if (start == NULL)
		{
			SDL_mutexP(gameData->task->statusMutex);
			gameData->task->status = SP_C4A_ERROR;
			SDL_mutexV(gameData->task->statusMutex);
			return 1;
		}
		char* end = my_strchr( start, '}', '\"'); //ignore "text}"-parts
		if (start == NULL)
		{
			SDL_mutexP(gameData->task->statusMutex);
			gameData->task->status = SP_C4A_ERROR;
			SDL_mutexV(gameData->task->statusMutex);
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
		spNetC4AGamePointer next = *(gameData->game);
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
			(*(gameData->game)) = new_game;
		}
		else
		{
			before->next = new_game;
			new_game->next = next;
		}
	}
	SDL_mutexP(gameData->task->statusMutex);
	gameData->task->status = SP_C4A_OK;
	SDL_mutexV(gameData->task->statusMutex);
	return 0;
}

PREFIX int spNetC4AGetGame(spNetC4AGamePointer* gameList,int timeOut)
{
	(*gameList) = NULL;
	SDL_mutexP(spGlobalC4ATask->statusMutex);
	if (spGlobalC4ATask->status != SP_C4A_PROGRESS)
	{
		spGlobalC4ATask->status = SP_C4A_PROGRESS;
		SDL_mutexV(spGlobalC4ATask->statusMutex);
		//Starting a background thread, which does the fancy stuff
		getgamePointer data = (getgamePointer)malloc(sizeof(getgameType));
		data->function = c4a_getgame_thread;
		data->task = spGlobalC4ATask;
		data->game = gameList;
		spGlobalC4ATask->dataPointer = data;
		spGlobalC4ATask->timeOut = timeOut;
		spGlobalC4ATask->threadStatus = 1;
		#ifdef _MSC_VER
			spGlobalC4ATask->thread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,data);
		#else
			spGlobalC4ATask->thread = SDL_CreateThread((int (*)(void *))spNetC4AUberThread,data);
		#endif
		return 0;
	}
	SDL_mutexV(spGlobalC4ATask->statusMutex);
	return 1;
}

PREFIX spNetC4ATaskPointer spNetC4AGetGameParallel(spNetC4AGamePointer* gameList,int timeOut)
{
	(*gameList) = NULL;
	spNetC4ATaskPointer task = createNewC4ATask();
	task->status = SP_C4A_PROGRESS;
	//Starting a background thread, which does the fancy stuff
	getgamePointer data = (getgamePointer)malloc(sizeof(getgameType));
	data->function = c4a_getgame_thread;
	data->task = task;
	data->game = gameList;
	task->dataPointer = data;
	task->timeOut = timeOut;
	task->threadStatus = 1;
	#ifdef _MSC_VER
		task->thread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,data);
	#else
		task->thread = SDL_CreateThread((int (*)(void *))spNetC4AUberThread,data);
	#endif
	return task;
}

int c4a_getscore_thread(void* data)
{
	getscorePointer scoreData = (getscorePointer)data;
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	if (ip.address.ipv4 == SP_INVALID_IP)
	{
		SDL_mutexP(scoreData->task->statusMutex);
		scoreData->task->status = SP_C4A_ERROR;
		SDL_mutexV(scoreData->task->statusMutex);
		return 1;
	}
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		SDL_mutexP(scoreData->task->statusMutex);
		scoreData->task->status = SP_C4A_ERROR;
		SDL_mutexV(scoreData->task->statusMutex);
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
		SDL_mutexP(scoreData->task->statusMutex);
		scoreData->task->status = SP_C4A_ERROR;
		SDL_mutexV(scoreData->task->statusMutex);
		return 1;
	}
	//skeezix saves the top500. So 100 byte should be enough...
	//Haha. NOT! minislug had 50950 with a top 500...
	char buffer[100001];
	int length;
	if ((length = spNetReceiveHTTP(connection,buffer,100000)) == 0)
	{
		spNetCloseTCP(connection);
		SDL_mutexP(scoreData->task->statusMutex);
		scoreData->task->status = SP_C4A_ERROR;
		SDL_mutexV(scoreData->task->statusMutex);
		return 1;
	}
	buffer[length] = 0;
	spNetCloseTCP(connection);
	//Searching the first [
	char* found = strchr( buffer, '[' );
	if (found == NULL)
	{
		SDL_mutexP(scoreData->task->statusMutex);
		scoreData->task->status = SP_C4A_ERROR;
		SDL_mutexV(scoreData->task->statusMutex);
		return 1;
	}
	//Reading score by score
	//Searching the starting {
	int rank = 1;
	spNetC4AScorePointer lastScore = NULL;
	while (found)
	{
		char* start = strchr( found, '{' );
		if (start == NULL)
		{
			SDL_mutexP(scoreData->task->statusMutex);
			scoreData->task->status = SP_C4A_ERROR;
			SDL_mutexV(scoreData->task->statusMutex);
			return 1;
		}
		char* end = my_strchr( start, '}', '\"'); //ignore "text}"-parts
		if (start == NULL)
		{
			SDL_mutexP(scoreData->task->statusMutex);
			scoreData->task->status = SP_C4A_ERROR;
			SDL_mutexV(scoreData->task->statusMutex);
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
		Uint64 commitTime = (Uint64)(internal_spNet_spAtoFloat(pos)); //float becase of bigger numbers

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
		new_score->rank = rank;
		if (lastScore == NULL)
			(*(scoreData->score)) = new_score;
		else
			lastScore->next = new_score;
		lastScore = new_score;
		rank++;
	}

	SDL_mutexP(scoreData->task->statusMutex);
	scoreData->task->status = SP_C4A_OK;
	SDL_mutexV(scoreData->task->statusMutex);
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
	SDL_mutexP(spGlobalC4ATask->statusMutex);
	if (spGlobalC4ATask->status != SP_C4A_PROGRESS)
	{
		spGlobalC4ATask->status = SP_C4A_PROGRESS;
		SDL_mutexV(spGlobalC4ATask->statusMutex);
		//Starting a background thread, which does the fancy stuff
		getscorePointer data = (getscorePointer)malloc(sizeof(getscoreType));
		data->function = c4a_getscore_thread;
		data->task = spGlobalC4ATask;
		data->score = scoreList;
		data->profile = profile;
		data->year = 0;
		data->month = 0;
		sprintf(data->game,"%s",game);
		spGlobalC4ATask->dataPointer = data;
		spGlobalC4ATask->timeOut = timeOut;
		spGlobalC4ATask->threadStatus = 1;
		#ifdef _MSC_VER
			spGlobalC4ATask->thread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,data);
		#else
			spGlobalC4ATask->thread = SDL_CreateThread((int (*)(void *))spNetC4AUberThread,data);
		#endif
		return 0;
	}
	SDL_mutexV(spGlobalC4ATask->statusMutex);
	return 1;
}

PREFIX int spNetC4AGetScoreOfMonth(spNetC4AScorePointer* scoreList,spNetC4AProfilePointer profile,char* game,int year,int month,int timeOut)
{
	(*scoreList) = NULL;
	if (month < 1 || month > 12)
		return 1;
	SDL_mutexP(spGlobalC4ATask->statusMutex);
	if (spGlobalC4ATask->status != SP_C4A_PROGRESS)
	{
		spGlobalC4ATask->status = SP_C4A_PROGRESS;
		SDL_mutexV(spGlobalC4ATask->statusMutex);
		//Starting a background thread, which does the fancy stuff
		getscorePointer data = (getscorePointer)malloc(sizeof(getscoreType));
		data->function = c4a_getscore_thread;
		data->task = spGlobalC4ATask;
		data->score = scoreList;
		data->profile = profile;
		data->year = year;
		data->month = month;
		sprintf(data->game,"%s",game);
		spGlobalC4ATask->dataPointer = data;
		spGlobalC4ATask->timeOut = timeOut;
		spGlobalC4ATask->threadStatus = 1;
		#ifdef _MSC_VER
			spGlobalC4ATask->thread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,data);
		#else
			spGlobalC4ATask->thread = SDL_CreateThread((int (*)(void *))spNetC4AUberThread,data);
		#endif
		return 0;
	}
	SDL_mutexV(spGlobalC4ATask->statusMutex);
	return 1;
}

PREFIX spNetC4ATaskPointer spNetC4AGetScoreParallel(spNetC4AScorePointer* scoreList,spNetC4AProfilePointer profile,char* game,int timeOut)
{
	(*scoreList) = NULL;
	spNetC4ATaskPointer task = createNewC4ATask();
	task->status = SP_C4A_PROGRESS;
	//Starting a background thread, which does the fancy stuff
	getscorePointer data = (getscorePointer)malloc(sizeof(getscoreType));
	data->function = c4a_getscore_thread;
	data->task = task;
	data->score = scoreList;
	data->profile = profile;
	data->year = 0;
	data->month = 0;
	sprintf(data->game,"%s",game);
	task->dataPointer = data;
	task->timeOut = timeOut;
	task->threadStatus = 1;
	#ifdef _MSC_VER
		task->thread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,data);
	#else
		task->thread = SDL_CreateThread((int (*)(void *))spNetC4AUberThread,data);
	#endif
	return task;
}

PREFIX spNetC4ATaskPointer spNetC4AGetScoreOfMonthParallel(spNetC4AScorePointer* scoreList,spNetC4AProfilePointer profile,char* game,int year,int month,int timeOut)
{
	(*scoreList) = NULL;
	if (month < 1 || month > 12)
		return NULL;
	spNetC4ATaskPointer task = createNewC4ATask();
	task->status = SP_C4A_PROGRESS;
	SDL_mutexV(task->statusMutex);
	//Starting a background thread, which does the fancy stuff
	getscorePointer data = (getscorePointer)malloc(sizeof(getscoreType));
	data->function = c4a_getscore_thread;
	data->task = task;
	data->score = scoreList;
	data->profile = profile;
	data->year = year;
	data->month = month;
	sprintf(data->game,"%s",game);
	task->dataPointer = data;
	task->timeOut = timeOut;
	task->threadStatus = 1;
	#ifdef _MSC_VER
		task->thread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,data);
	#else
		task->thread = SDL_CreateThread((int (*)(void *))spNetC4AUberThread,data);
	#endif
	return task;
}

PREFIX void spNetC4AFilterScore(spNetC4AScorePointer* scoreList)
{
	spNetC4AScorePointer mom = *scoreList;
	spNetC4AScorePointer before = NULL;
	while (mom)
	{
		//Search for mom:
		spNetC4AScorePointer inner = *scoreList;
		while (inner != NULL && inner != mom)
		{
			if (strcmp(mom->longname,inner->longname) == 0 &&
				strcmp(mom->shortname,inner->shortname) == 0)
				break;
			inner = inner->next;
		}
		spNetC4AScorePointer next = mom->next;
		//already there!
		if (inner != mom)
		{
			if (before)
				before->next = next;
			free(mom);
		}
		else
			before = mom;
		mom = next;
	}
}

static int do_the_real_c4a_commit(spNetIP ip,commitPointer commitData,char* game,char* system,char* prid,int score)
{
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
		return 1;
	char commit_string[2048];
	sprintf(commit_string,"PUT /plugtally_1/scoreonly/%s/%s/%s?score=%i HTTP/1.1\r\nHost: skeezix.wallednetworks.com:13001\r\nAccept: */*\r\nContent-Length: 0\r\nExpect: 100-continue\r\n",game,system,prid,score);
	if (spNetSendHTTP(connection,commit_string) == 0)
	{
		spNetCloseTCP(connection);
		return 1;
	}
	spNetCloseTCP(connection);
	//Adding to scoreList ;)
	if (commitData->scoreList)
	{
		spNetC4AScorePointer new_score = (spNetC4AScorePointer)malloc(sizeof(spNetC4AScore));
		sprintf(new_score->longname,"%s",commitData->profile->longname);
		sprintf(new_score->shortname,"%s",commitData->profile->shortname);
		new_score->score = score;
		new_score->commitTime = time(NULL);
		new_score->next = (*(commitData->scoreList));
		(*(commitData->scoreList)) = new_score;
	}
	return 0;
}


#ifndef __GNUC__
	#include <windows.h>
	#include <tchar.h>
#else
	#include <dirent.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif

int c4a_commit_thread(void* data)
{
	commitPointer commitData = (commitPointer)data;
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	if (ip.address.ipv4 == SP_INVALID_IP)
	{
		if (spNetC4ACaching && commitData->game[0] != 0)
			write_to_cache(commitData->game,commitData->system,commitData->profile->prid,commitData->score,1);
		SDL_mutexP(commitData->task->statusMutex);
		commitData->task->status = SP_C4A_ERROR;
		SDL_mutexV(commitData->task->statusMutex);
		return 1;
	}
	if (commitData->game[0] != 0)
	{
		if (do_the_real_c4a_commit(ip,commitData,commitData->game,commitData->system,commitData->profile->prid,commitData->score))
		{
			if (spNetC4ACaching)
				write_to_cache(commitData->game,commitData->system,commitData->profile->prid,commitData->score,1);
			SDL_mutexP(commitData->task->statusMutex);
			commitData->task->status = SP_C4A_ERROR;
			SDL_mutexV(commitData->task->statusMutex);
			return 1;
		}
	}
	//Checking for stuff in the cache
	SDL_mutexP(spCacheMutex);
	cachePointer cache = read_cache();
	if (cache)
	{
		while (cache)
		{
			cachePointer next = cache->next;
			if (do_the_real_c4a_commit(ip,commitData,cache->game,cache->system,cache->prid,cache->score))
				break;
			free(cache);
			cache = next;
			if (commitData->task->message == 1)
			{
				commitData->task->message = 2;
				break;
			}
		}
		#ifdef _WIN32
			DeleteFile(spCacheFilename);
		#else
			remove(spCacheFilename);
		#endif
		//if cache is still existing, we need to write back the rest of the cache
		if (cache)
		{
			SDL_RWops *file = SDL_RWFromFile(spCacheFilename, "ab");
			while (cache)
			{
				cachePointer next = cache->next;
				SDL_RWwrite(file,cache->game,256,1);
				SDL_RWwrite(file,cache->system,256,1);
				SDL_RWwrite(file,cache->prid,256,1);
				SDL_RWwrite(file,&(cache->score),sizeof(int),1);
				free(cache);
				cache = next;
			}
			SDL_RWclose(file);
		}

	}
	SDL_mutexV(spCacheMutex);

	SDL_mutexP(commitData->task->statusMutex);
	commitData->task->status = SP_C4A_OK;
	SDL_mutexV(commitData->task->statusMutex);
	return 0;
}

PREFIX int spNetC4AHowManyCached( void )
{
	int result = 0;
	SDL_mutexP(spCacheMutex);
	SDL_RWops *file = SDL_RWFromFile(spCacheFilename, "rb");
	if (file)
	{
		char buffer[256*3+sizeof(int)];
		while (1)
		{
			if (SDL_RWread(file,buffer,256*3+sizeof(int),1) <= 0)
				break;
			result++;
		}
		SDL_RWclose(file);
	}
	SDL_mutexV(spCacheMutex);
	return result;
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

#ifdef GP2X
	#define SET_SYSTEM(system) sprintf(system,"gp2x");
#elif defined(CAANOO)
	#define SET_SYSTEM(system) sprintf(system,"caanoo");
#elif defined(WIZ)
	#define SET_SYSTEM(system) sprintf(system,"wiz");
#elif defined(DINGUX)
	#define SET_SYSTEM(system) sprintf(system,"dingux");
#elif defined(GCW)
	#define SET_SYSTEM(system) sprintf(system,"gcw");
#elif defined(PANDORA)
	#define SET_SYSTEM(system) sprintf(system,"pandora");
#elif defined(_WIN32)
	#define SET_SYSTEM(system) sprintf(system,"win32");
#else
	#define SET_SYSTEM(system) sprintf(system,"linux");
#endif


PREFIX int spNetC4ACommitScore(spNetC4AProfilePointer profile,char* game,int score,spNetC4AScorePointer* scoreList,int timeOut)
{
	if (profile == NULL && game[0]!=0)
		return 1;
	int already = 0;
	if (scoreList && already_in_highscore(*scoreList,profile,score))
		already = 1;
	if (timeOut == 0)
	{
		if (already == 0 && spNetC4ACaching)
		{
			char system[256];
			SET_SYSTEM(system);
			write_to_cache(game,system,profile->prid,score,1);
		}
		return 1;
	}
	SDL_mutexP(spGlobalC4ATask->statusMutex);
	if (spGlobalC4ATask->status != SP_C4A_PROGRESS)
	{
		spGlobalC4ATask->status = SP_C4A_PROGRESS;
		SDL_mutexV(spGlobalC4ATask->statusMutex);
		//Starting a background thread, which does the fancy stuff
		commitPointer data = (commitPointer)malloc(sizeof(commitType));
		data->task = spGlobalC4ATask;
		data->function = c4a_commit_thread;
		data->score = score;
		data->profile = profile;
		data->scoreList = scoreList;
		if (already)
			data->game[0] = 0;
		else
			sprintf(data->game,"%s",game);
		SET_SYSTEM(data->system);
		spGlobalC4ATask->dataPointer = data;
		spGlobalC4ATask->timeOut = timeOut;
		spGlobalC4ATask->threadStatus = 1;
		#ifdef _MSC_VER
			spGlobalC4ATask->thread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,data);
		#else
			spGlobalC4ATask->thread = SDL_CreateThread((int (*)(void *))spNetC4AUberThread,data);
		#endif
		return already;
	}
	SDL_mutexV(spGlobalC4ATask->statusMutex);
	return 1;
}

PREFIX spNetC4ATaskPointer spNetC4ACommitScoreParallel(spNetC4AProfilePointer profile,char* game,int score,spNetC4AScorePointer* scoreList,int timeOut)
{
	if (profile == NULL)
		return NULL;
	int already = 0;
	if (scoreList && already_in_highscore(*scoreList,profile,score))
		already = 1;
	if (timeOut == 0)
	{
		if (already == 0 && spNetC4ACaching)
		{
			char system[256];
			SET_SYSTEM(system);
			write_to_cache(game,system,profile->prid,score,1);
		}
		return NULL;
	}
	spNetC4ATaskPointer task = createNewC4ATask();
	task->status = SP_C4A_PROGRESS;
	//Starting a background thread, which does the fancy stuff
	commitPointer data = (commitPointer)malloc(sizeof(commitType));
	data->task = task;
	data->function = c4a_commit_thread;
	data->score = score;
	data->profile = profile;
	data->scoreList = scoreList;
	if (already)
		data->game[0] = 0;
	else
		sprintf(data->game,"%s",game);
	SET_SYSTEM(data->system);
	task->dataPointer = data;
	task->timeOut = timeOut;
	task->threadStatus = 1;
	#ifdef _MSC_VER
		task->thread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,data);
	#else
		task->thread = SDL_CreateThread((int (*)(void *))spNetC4AUberThread,data);
	#endif
	return task;
}

PREFIX void spNetC4ACopyScoreList(spNetC4AScorePointer* scoreList,spNetC4AScorePointer* newList)
{
	if (scoreList == NULL)
		return;
	if (newList == NULL)
		return;
	spNetC4AScorePointer mom = *scoreList;
	spNetC4AScorePointer last = NULL;
	while (mom)
	{
		spNetC4AScorePointer copy_score = (spNetC4AScorePointer)malloc(sizeof(spNetC4AScore));
		sprintf(copy_score->longname,"%s",mom->longname);
		sprintf(copy_score->shortname,"%s",mom->shortname);
		copy_score->score = mom->score;
		copy_score->commitTime = mom->commitTime;
		copy_score->rank = mom->rank;
		if (last)
			last->next = copy_score;
		else
			*newList = copy_score;
		last = copy_score;
		mom = mom->next;
	}
	if (last)
		last->next = NULL;
	else
		*newList = NULL;
}

typedef struct __ScoreNameStruct *__ScoreNamePointer;
typedef struct __ScoreNameStruct {
	char longname[256];
	char shortname[256];
	__ScoreNamePointer next;
} __ScoreName;

PREFIX void spNetC4AMakeScoresUnique(spNetC4AScorePointer* scoreList)
{
	if (scoreList == NULL)
		return;
	spNetC4AScorePointer mom = *scoreList;
	spNetC4AScorePointer before = NULL;
	__ScoreNamePointer name = NULL;
	__ScoreNamePointer searchStart = NULL;
	while (mom)
	{
		//search mom in name:
		__ScoreNamePointer search = searchStart;
		while (search)
		{
			if (strcmp(mom->shortname,search->shortname) == 0 &&
				strcmp(mom->longname,search->longname) == 0 )
				break; //found
			search = search->next;
		}
		if (search) //found -> remove
		{
			spNetC4AScorePointer next = mom->next;
			before->next = next;
			free(mom);
			mom = next;
		}
		else //add
		{
			__ScoreNamePointer add = (__ScoreNamePointer)malloc(sizeof(__ScoreName));
			sprintf(add->longname,"%s",mom->longname);
			sprintf(add->shortname,"%s",mom->shortname);
			add->next = searchStart;
			searchStart = add;
			before = mom;
			mom = mom->next;
		}
	}
	while (searchStart)
	{
		__ScoreNamePointer next = searchStart->next;
		free(searchStart);
		searchStart = next;
	}
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

int c4a_create_thread(void* data)
{
	createPointer createData = (createPointer)data;
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	if (ip.address.ipv4 == SP_INVALID_IP)
	{
		SDL_mutexP(createData->task->statusMutex);
		createData->task->status = SP_C4A_ERROR;
		SDL_mutexV(createData->task->statusMutex);
		return 1;
	}
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		SDL_mutexP(createData->task->statusMutex);
		createData->task->status = SP_C4A_ERROR;
		SDL_mutexV(createData->task->statusMutex);
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
		SDL_mutexP(createData->task->statusMutex);
		createData->task->status = SP_C4A_ERROR;
		SDL_mutexV(createData->task->statusMutex);
		return 1;
	}
	if (spNetSendTCP(connection,create_string,strlen(create_string)) == 0)
	{
		spNetCloseTCP(connection);
		SDL_mutexP(createData->task->statusMutex);
		createData->task->status = SP_C4A_ERROR;
		SDL_mutexV(createData->task->statusMutex);
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
	SDL_mutexP(createData->task->statusMutex);
	createData->task->status = SP_C4A_OK;
	SDL_mutexV(createData->task->statusMutex);
	return 0;
}

PREFIX int spNetC4ACreateProfile(spNetC4AProfilePointer* profile, char* longname,char* shortname,char* password,char* email,int timeOut)
{
	if (profile == NULL)
		return 1;
	SDL_mutexP(spGlobalC4ATask->statusMutex);
	if (spGlobalC4ATask->status != SP_C4A_PROGRESS)
	{
		spGlobalC4ATask->status = SP_C4A_PROGRESS;
		SDL_mutexV(spGlobalC4ATask->statusMutex);
		//Starting a background thread, which does the fancy stuff
		createPointer data = (createPointer)malloc(sizeof(createType));
		data->task = spGlobalC4ATask;
		data->function = c4a_create_thread;
		data->profile = profile;
		sprintf(data->longname,"%s",longname);
		sprintf(data->shortname,"%s",shortname);
		sprintf(data->password,"%s",password);
		sprintf(data->email,"%s",email);
		spGlobalC4ATask->dataPointer = data;
		spGlobalC4ATask->timeOut = timeOut;
		spGlobalC4ATask->threadStatus = 1;
		#ifdef _MSC_VER
			spGlobalC4ATask->thread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,data);
		#else
			spGlobalC4ATask->thread = SDL_CreateThread((int (*)(void *))spNetC4AUberThread,data);
		#endif
		return 0;
	}
	SDL_mutexV(spGlobalC4ATask->statusMutex);
	return 1;
}

int c4a_delete_thread(void* data)
{
	createPointer createData = (createPointer)data;
	spNetIP ip = spNetResolve("skeezix.wallednetworks.com",13001);
	if (ip.address.ipv4 == SP_INVALID_IP)
	{
		SDL_mutexP(createData->task->statusMutex);
		createData->task->status = SP_C4A_ERROR;
		SDL_mutexV(createData->task->statusMutex);
		return 1;
	}
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		SDL_mutexP(createData->task->statusMutex);
		createData->task->status = SP_C4A_ERROR;
		SDL_mutexV(createData->task->statusMutex);
		return 1;
	}
	char create_string[2048];
	char buffer[2048];
	sprintf(create_string,"{\"email\": \"%s\", \"shortname\": \"%s\", \"password\": \"%s\", \"prid\": \"%s\", \"longname\": \"%s\"}",(*(createData->profile))->email,(*(createData->profile))->shortname,(*(createData->profile))->password,(*(createData->profile))->prid,(*(createData->profile))->longname);
	sprintf(buffer,"PUT /delprofile_1 HTTP/1.1\r\nUser-Agent: sparrowNet/1.0\r\nHost: %i.%i.%i.%i:13001\r\nAccept: */*\r\nContent-Length: %i\r\nExpect: 100-continue\r\n\r\n",ip.address.ipv4_bytes[0],ip.address.ipv4_bytes[1],ip.address.ipv4_bytes[2],ip.address.ipv4_bytes[3],strlen(create_string));
	if (spNetSendTCP(connection,buffer,strlen(buffer)) == 0)
	{
		spNetCloseTCP(connection);
		SDL_mutexP(createData->task->statusMutex);
		createData->task->status = SP_C4A_ERROR;
		SDL_mutexV(createData->task->statusMutex);
		return 1;
	}
	if (spNetSendTCP(connection,create_string,strlen(create_string)) == 0)
	{
		spNetCloseTCP(connection);
		SDL_mutexP(createData->task->statusMutex);
		createData->task->status = SP_C4A_ERROR;
		SDL_mutexV(createData->task->statusMutex);
		return 1;
	}
	spNetCloseTCP(connection);
	(*(createData->profile)) = NULL;
	if (createData->deleteFile)
		spNetC4ADeleteProfileFile();
	SDL_mutexP(createData->task->statusMutex);
	createData->task->status = SP_C4A_OK;
	SDL_mutexV(createData->task->statusMutex);
	return 0;
}

PREFIX int spNetC4ADeleteAccount(spNetC4AProfilePointer* profile,int deleteFile,int timeOut)
{
	if (profile == NULL)
		return 1;
	SDL_mutexP(spGlobalC4ATask->statusMutex);
	if (spGlobalC4ATask->status != SP_C4A_PROGRESS)
	{
		spGlobalC4ATask->status = SP_C4A_PROGRESS;
		SDL_mutexV(spGlobalC4ATask->statusMutex);
		//Starting a background thread, which does the fancy stuff
		createPointer data = (createPointer)malloc(sizeof(createType));
		data->task = spGlobalC4ATask;
		data->function = c4a_delete_thread;
		data->profile = profile;
		data->deleteFile = deleteFile;
		spGlobalC4ATask->dataPointer = data;
		spGlobalC4ATask->timeOut = timeOut;
		spGlobalC4ATask->threadStatus = 1;
		#ifdef _MSC_VER
			spGlobalC4ATask->thread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,data);
		#else
			spGlobalC4ATask->thread = SDL_CreateThread((int (*)(void *))spNetC4AUberThread,data);
		#endif
		return 0;
	}
	SDL_mutexV(spGlobalC4ATask->statusMutex);
	return 1;
}

PREFIX void spNetC4ADeleteProfileFile( void )
{
	PROFILE_FILENAME_MAKRO
//Copied from spRemoveFile to avoid dependencies
#ifdef _WIN32
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
		SDL_mutexP(createData->task->statusMutex);
		createData->task->status = SP_C4A_ERROR;
		SDL_mutexV(createData->task->statusMutex);
		return 1;
	}
	spNetTCPConnection connection = spNetOpenClientTCP(ip);
	if (connection == NULL)
	{
		SDL_mutexP(createData->task->statusMutex);
		createData->task->status = SP_C4A_ERROR;
		SDL_mutexV(createData->task->statusMutex);
		return 1;
	}
	char create_string[2048];
	char buffer[2048];
	sprintf(create_string,"{\"email\": \"%s\", \"shortname\": \"%s\", \"password\": \"%s\", \"prid\": \"%s\", \"longname\": \"%s\"}",createData->email,createData->shortname,createData->password,(*(createData->profile))->prid,createData->longname);
	sprintf(buffer,"PUT /setprofile_1 HTTP/1.1\r\nUser-Agent: sparrowNet/1.0\r\nHost: %i.%i.%i.%i:13001\r\nAccept: */*\r\nContent-Length: %i\r\nExpect: 100-continue\r\n\r\n",ip.address.ipv4_bytes[0],ip.address.ipv4_bytes[1],ip.address.ipv4_bytes[2],ip.address.ipv4_bytes[3],strlen(create_string));
	if (spNetSendTCP(connection,buffer,strlen(buffer)) == 0)
	{
		spNetCloseTCP(connection);
		SDL_mutexP(createData->task->statusMutex);
		createData->task->status = SP_C4A_ERROR;
		SDL_mutexV(createData->task->statusMutex);
		return 1;
	}
	if (spNetSendTCP(connection,create_string,strlen(create_string)) == 0)
	{
		spNetCloseTCP(connection);
		SDL_mutexP(createData->task->statusMutex);
		createData->task->status = SP_C4A_ERROR;
		SDL_mutexV(createData->task->statusMutex);
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
	SDL_mutexP(createData->task->statusMutex);
	createData->task->status = SP_C4A_OK;
	SDL_mutexV(createData->task->statusMutex);
	return 0;
}

PREFIX int spNetC4AEditProfile(spNetC4AProfilePointer* profile,char* longname,char* shortname,char* password,char* email,int timeOut)
{
	if (profile == NULL)
		return 1;
	SDL_mutexP(spGlobalC4ATask->statusMutex);
	if (spGlobalC4ATask->status != SP_C4A_PROGRESS)
	{
		spGlobalC4ATask->status = SP_C4A_PROGRESS;
		SDL_mutexV(spGlobalC4ATask->statusMutex);
		//Starting a background thread, which does the fancy stuff
		createPointer data = (createPointer)malloc(sizeof(createType));
		data->task = spGlobalC4ATask;
		data->function = c4a_edit_thread;
		data->profile = profile;
		sprintf(data->longname,"%s",longname);
		sprintf(data->shortname,"%s",shortname);
		sprintf(data->password,"%s",password);
		sprintf(data->email,"%s",email);
		spGlobalC4ATask->dataPointer = data;
		spGlobalC4ATask->timeOut = timeOut;
		spGlobalC4ATask->threadStatus = 1;
		#ifdef _MSC_VER
			spGlobalC4ATask->thread = SDL_CreateThread((int (__cdecl *)(void *))spNetC4AUberThread,data);
		#else
			spGlobalC4ATask->thread = SDL_CreateThread((int (*)(void *))spNetC4AUberThread,data);
		#endif
		return 0;
	}
	SDL_mutexV(spGlobalC4ATask->statusMutex);
	return 1;
}

PREFIX int spNetC4AGetStatusParallel(spNetC4ATaskPointer task)
{
	SDL_mutexP(task->statusMutex);
	if (task->threadStatus)
	{
		SDL_mutexV(task->statusMutex);
		return SP_C4A_PROGRESS;
	}
	int status = task->status;
	SDL_mutexV(task->statusMutex);
	return status;
}

PREFIX int spNetC4AGetStatus( void )
{
	return spNetC4AGetStatusParallel(spGlobalC4ATask);
}

PREFIX void spNetC4ACancelTaskParallel(spNetC4ATaskPointer task)
{
	SDL_mutexP(task->statusMutex);
	if (task->status > 0)
	{
		task->status = SP_C4A_CANCELED;
		SDL_mutexV(task->statusMutex);
		SDL_WaitThread(task->thread,NULL);
	}
	else
		SDL_mutexV(task->statusMutex);
}

PREFIX void spNetC4ACancelTask( void )
{
	spNetC4ACancelTaskParallel(spGlobalC4ATask);
}

PREFIX int spNetC4AGetTaskResult( void )
{
	return spGlobalC4ATask->result;
}

PREFIX int spNetC4AGetTaskResultParallel(spNetC4ATaskPointer task)
{
	return task->result;
}

PREFIX int spNetC4AGetTimeOut( void )
{
	return spGlobalC4ATask->timeOut;
}

PREFIX int spNetC4AGetTimeOutParallel(spNetC4ATaskPointer task)
{
	return task->timeOut;
}

// IRC:

PREFIX void spNetIRCSend(spNetIRCServerPointer server,char* message)
{
	char buffer[1024];
	sprintf(buffer,"%s\n",message);
	spNetSendTCP(server->connection,buffer,strlen(buffer));
}

char __irc_upper_case(char c)
{
	if ((c >= 'a') && (c <= 'z'))
		c -= 32;
	return c;
}

char* __irc_upper_case_string(char* input,char* output)
{
	int i;
	for (i = 0; input[i]; i++)
		output[i] = __irc_upper_case(input[i]);
	output[i] = 0;
	return output;
}

void __irc_add_nick(spNetIRCChannelPointer channel,char* name,char rights)
{
	//if (strcmp(name,"ChanServ") == 0)
	//	return;
	spNetIRCNickPointer nick = (spNetIRCNickPointer)malloc(sizeof(spNetIRCNick));
	sprintf(nick->name,"%s",name);
	nick->rights = rights;
	char buffer1[256],buffer2[256];
	if (channel->first_nick == NULL || strcmp(__irc_upper_case_string(channel->first_nick->name,buffer1),__irc_upper_case_string(nick->name,buffer2)) > 0)
	{
		nick->next = channel->first_nick;
		channel->first_nick = nick;
		return;
	}
	spNetIRCNickPointer before = channel->first_nick;
	while (before->next && strcmp(__irc_upper_case_string(before->next->name,buffer1),__irc_upper_case_string(nick->name,buffer2)) < 0)
		before = before->next;
	nick->next = before->next;
	before->next = nick;
}

spNetIRCChannelPointer __irc_add_channel(spNetIRCServerPointer server,char* name,int* already)
{
	char namestring[512];
	sprintf(namestring,"%s",name);
	char *pos;
	if (pos = strchr(namestring,' '))
		pos[0] = 0;
	spNetIRCChannelPointer mom = server->first_channel;
	while (mom)
	{
		if (strcmp(mom->name,namestring) == 0)
			break;
		mom = mom->next;
	}
	if (mom)
	{
		*already = 1;
		return mom;
	}
	*already = 0;
	spNetIRCChannelPointer channel = (spNetIRCChannelPointer)malloc(sizeof(spNetIRCChannel));
	channel->status = 0;
	channel->first_message = NULL;
	channel->last_message = NULL;
	channel->last_read_message = NULL;
	channel->first_add_message = NULL;
	channel->last_add_message = NULL;
	channel->last_add_read_message = NULL;
	channel->first_nick = NULL;
	channel->got_end_366 = 0;
	channel->next = NULL;
	channel->close_query = 0;
	channel->show_users = 0;
	sprintf(channel->name,"%s",namestring);
	if (server->last_channel)
		server->last_channel->next = channel;
	else
		server->first_channel = channel;
	server->last_channel = channel;
	if (name[0] != '#') //query
	{
		__irc_add_nick(channel,namestring,' ');
		__irc_add_nick(channel,server->nickname,' ');
		channel->status = 1;
	}
	return channel;
}

void __irc_remove_channel(spNetIRCServerPointer server,spNetIRCChannelPointer channel)
{
	spNetIRCChannelPointer mom = server->first_channel;
	spNetIRCChannelPointer before = NULL;
	while (mom)
	{
		if (mom == channel)
			break;
		before = mom;
		mom = mom->next;
	}
	while (channel->first_add_message)
	{
		spNetIRCMessagePointer next = channel->first_add_message->next;
		free(channel->first_add_message);
		channel->first_add_message = next;
	}
	while (channel->first_message)
	{
		spNetIRCMessagePointer next = channel->first_message->next;
		free(channel->first_message);
		channel->first_message = next;
	}
	if (mom)
	{
		if (before)
			before->next = mom->next;
		else
			server->first_channel = mom->next;
		if (mom == server->last_channel)
			server->last_channel = before;
	}
}

spNetIRCMessagePointer __irc_add_message(spNetIRCServerPointer server,spNetIRCMessagePointer* first_message,spNetIRCMessagePointer* last_message,char* type,char* message,char* user)
{
	spNetIRCMessagePointer msg = (spNetIRCMessagePointer)malloc(sizeof(spNetIRCMessage));
	msg->ctcp[0] = 0;
	if (message[0] == 1) //ctcp
	{
		char* end = strchr(&message[1],1);
		if (end)
			end[0] = 0;
		int i;
		for (i = 0; i < 15;i++)
		{
			message++;
			if (message[0] != ' ')
				msg->ctcp[i] = message[0];
			else
				break;
		}
		if (message[0] == ' ')
			message++;
		msg->ctcp[i] = 0;
	}
	sprintf(msg->type,"%s",type);
	if (user)
		sprintf(msg->user,"%s",user);
	else
		sprintf(msg->user,"%s",server->name);
	sprintf(msg->message,"%s",message);
	msg->next = NULL;
	if (*last_message)
		(*last_message)->next = msg;
	else
		*first_message = msg;
	*last_message = msg;
	msg->time_stamp = time(NULL);
	return msg;
}

void __irc_split_user_destiny(char** parameters,char** user,char** prefix,char** destiny)
{
	*user = *prefix;
	(*user)++;
	char* end_of_user = strchr(*prefix,'!');
	if (end_of_user) //user found
		end_of_user[0] = 0;
	//extract destiny
	*destiny = *parameters;
	char* end = strchr(*parameters,' ');
	if (end)
	{
		end[0] = 0;
		*parameters = end;
		(*parameters)++;
	}
	else
		*parameters = &((*parameters)[strlen(*parameters)]);
}

int __irc_split_destiny_parameters(char** parameters,char** destiny)
{
	*destiny = strchr(*parameters,'#');
	if (!(*destiny))
		return 1;
	*parameters = strchr(*parameters,':');
	if (*parameters)
		(*parameters)++;
	else
		return 1;
	char* end = strchr(*destiny,' ');
	if (end)
		end[0] = 0;
	else
		return 1;
	return 0;
}


spNetIRCChannelPointer __irc_get_channel(spNetIRCServerPointer server,char* name)
{
	spNetIRCChannelPointer channel = server->first_channel;
	while (channel)
	{
		if (strcmp(channel->name,name) == 0)
			return channel;
		channel = channel->next;
	}
	return NULL;
}

void __irc_command_handling(spNetIRCServerPointer server,char* command,char* parameters,char* prefix)
{
	spNetIRCChannelPointer channel = NULL;
	char* user = NULL;
	char* destiny = NULL;
	if (strcmp(command,"PING") == 0)
	{
		char buffer[256];
		sprintf(buffer,"PONG %s",parameters);
		#ifdef DEBUG_MESSAGES
			printf("\t--> PONG %s\n",parameters);
		#endif
		spNetIRCSend(server,buffer);
	}
	if (strcmp(command,"JOIN") == 0)
	{
		__irc_split_user_destiny(&parameters,&user,&prefix,&destiny);
		//if (strcmp(user,"ChanServ") == 0)
		//	return;
		if (strcmp(user,server->nickname) == 0) //force join?
		{
			int already = 0;
			channel = __irc_add_channel(server,destiny,&already);
			if (already)
				channel->status = 1;
			return;
		}
		//Is the channel / user known?
		channel = __irc_get_channel(server,destiny);
		if (channel)
		{
			char buffer[256];
			sprintf(buffer,"%s joined the channel.",user);
			__irc_add_message(server,&(channel->first_message),&(channel->last_message),command,buffer,user);
			sprintf(buffer,"NAMES %s",channel->name);
			spNetIRCSend(server,buffer);
			return;
		}
	}
	if (strcmp(command,"PART") == 0)
	{
		__irc_split_user_destiny(&parameters,&user,&prefix,&destiny);
		//if (strcmp(user,"ChanServ") == 0)
		//	return;
		//Is the channel / user known?
		channel = __irc_get_channel(server,destiny);
		if (channel)
		{
			if (strcmp(user,server->nickname) == 0) //myself
				__irc_remove_channel(server,channel);
			else
			{
				char buffer[256];
				sprintf(buffer,"%s has left the channel.",user);
				__irc_add_message(server,&(channel->first_message),&(channel->last_message),command,buffer,user);
				sprintf(buffer,"NAMES %s",channel->name);
				spNetIRCSend(server,buffer);
			}
		}
		return;
	}
	if (strcmp(command,"QUIT") == 0)
	{
		user = prefix;
		user++;
		char* end_of_user = strchr(prefix,'!');
		if (end_of_user) //user found
			end_of_user[0] = 0;
		channel = server->first_channel;
		while (channel)
		{
			spNetIRCNickPointer nick = channel->first_nick;
			while (nick)
			{
				if (strcmp(nick->name,user) == 0)
					break;
				nick = nick->next;
			}
			if (nick)
			{
				char buffer[256];
				parameters = strchr(parameters,':');
				if (parameters)
				{
					char *plus = parameters;
					plus++;
					sprintf(buffer,"%s has quit (%s).",user,plus);
				}
				else
					sprintf(buffer,"%s has quit.",user);
				__irc_add_message(server,&(channel->first_message),&(channel->last_message),command,buffer,user);
				sprintf(buffer,"NAMES %s",channel->name);
				spNetIRCSend(server,buffer);
			}
			channel = channel->next;
		}
		return;
	}
	if (strcmp(command,"PRIVMSG") == 0)
	{
		__irc_split_user_destiny(&parameters,&user,&prefix,&destiny);
		//Is the channel / user known?
		char* channel_name;
		if (destiny[0] == '#')
			channel_name = destiny;
		else
			channel_name = user;
		channel = __irc_get_channel(server,channel_name);
		if (!channel)
		{
			int already;
			channel = __irc_add_channel(server,channel_name,&already);
		}
	}
	if (command[0] >= '0' && command[0] <= '9') //nr
	{
		char* begin;
		char* end;
		int nr = atoi(command);
		switch (nr) //See: https://www.alien.net.au/irc/irc2numerics.html
		{
			case 451:
				break;
			case 442: //PART of a not joined channel
				if (__irc_split_destiny_parameters(&parameters,&destiny))
					return;
				channel = __irc_get_channel(server,destiny);
				if (channel)
					__irc_remove_channel(server,channel);
				break;
			case 470: case 471: case 473: case 474: case 475:
				begin = strchr(parameters,'#');
				if (!begin)
					return;
				end = strchr(begin,' ');
				if (end)
					end[0] = 0;
				else
					return;
				channel = __irc_get_channel(server,begin);
				if (channel)
					channel->status = -1;
				return;
			case 433:case 043:case 436: case 437:
				server->status = 1;
				server->counter++;
				sprintf(server->nickname,"%s%i",server->original_nickname,server->counter);
				break;
			case 332: //topic
				if (__irc_split_destiny_parameters(&parameters,&destiny))
					return;
				//Is the channel / user known?
				channel = __irc_get_channel(server,destiny);
				if (channel)
					__irc_add_message(server,&(channel->first_message),&(channel->last_message),command,parameters,"Topic is");
				return;
			case 353: //nicks of the channel, furthermore reply to "names"
				if (__irc_split_destiny_parameters(&parameters,&destiny))
					return;
				channel = __irc_get_channel(server,destiny);
				if (!channel)
					return;
				if (channel->got_end_366)
					while (channel->first_nick)
					{
						spNetIRCNickPointer next = channel->first_nick->next;
						free(channel->first_nick);
						channel->first_nick = next;
					}
				channel->got_end_366 = 0;
				if (channel->show_users)
					__irc_add_message(server,&(channel->first_message),&(channel->last_message),command,parameters,"Users in channel are");
				begin = parameters;
				while (1)
				{
					end = strchr(begin,' ');
					if (end)
						end[0] = 0;
					char rights = ' ';
					if (begin[0] == '@' || begin[0] == '+')
					{
						rights = begin[0];
						begin++;
					}
					__irc_add_nick(channel,begin,rights);
					if (!end)
						break;
					begin = end;
					begin++;
				}
				return;
			case 366: //end of names => ignore
				if (__irc_split_destiny_parameters(&parameters,&destiny))
					return;
				//Is the channel / user known?
				channel = __irc_get_channel(server,destiny);
				if (channel)
					channel->got_end_366 = 1;
				return;
			default:
				if (server->status == 2)
					server->status = 3;
		}
	}
	parameters = strchr(parameters,':');
	if (parameters)
	{
		parameters++;
		if (channel)
			__irc_add_message(server,&(channel->first_message),&(channel->last_message),command,parameters,user);
		else
			__irc_add_message(server,&(server->first_message),&(server->last_message),command,parameters,user);
	}
}

void __irc_parse_one_line(spNetIRCServerPointer server,char* line)
{
	#ifdef DEBUG_MESSAGES
		printf("\t%s\n",line);
	#endif
	char* prefix = NULL;
	if (line[0] == ':') //comment thing, let's ignore it!
	{
		prefix = line;
		line = strchr(line,' ');
		line[0] = 0;
		if (line)
			line++;
		else
			return;
	}
	char* command = line;
	line = strchr(line,' ');
	if (line)
	{
		line[0] = 0;
		line++;
	}
	else
		return;
	__irc_command_handling(server,command,line,prefix);
}

void __irc_parse_result(spNetIRCServerPointer server,char* buffer)
{
	char* line = buffer;
	while (line)
	{
		int first = 0;
		char* endline = strchr(line,'\r');
		if (endline)
		{
			first = 1;
			endline[0] = 0;
		}
		else
		{
			endline = strchr(line,'\n');
			if (endline)
				endline[0] = 0;
			else
				break;
		}
		__irc_parse_one_line(server,line);
		if (first)
			endline++;
		line = endline;
		if (line)
			line++;
	}
}

int __irc_server_thread(void* data)
{
	spNetIRCServerPointer server = data;
	SDL_Thread* tcp_thread = NULL;
	char buffer[65536];
	sprintf(buffer,"PASS %s",server->password);
	spNetIRCSend(server,buffer);
	while (1)
	{
		if (server->status == 3)
		{
			sprintf(buffer,"PRIVMSG NickServ :IDENTIFY %s %s",server->nickname,server->password);
			spNetIRCSend(server,buffer);
			server->status++;
		}
		if (server->status == 0)
		{
			sprintf(buffer,"USER %s 8 * :%s",server->username,server->realname);
			spNetIRCSend(server,buffer);
			server->status++;
		}
		if (server->status == 1)
		{
			sprintf(buffer,"NICK %s",server->nickname);
			spNetIRCSend(server,buffer);
			server->status++;
			//Rejoin if needed
			spNetIRCChannelPointer channel = server->first_channel;
			while (channel)
			{
				if (channel->name[0] == '#') //real channel
				{
					char buffer[1024];
					sprintf(buffer,"JOIN %s",channel->name);
					spNetIRCSend(server,buffer);
				}
				channel = channel->next;
			}
		}

		int finish_flag = server->finish_flag; // same flag for whole loop run
		if (tcp_thread == NULL)
			tcp_thread = spNetReceiveTCPUnblocked(server->connection,buffer,65536);
		else
		{
			int l;
			if (l = spNetReceiveFinished(tcp_thread))
			{
				if (l > 0)
				{
					buffer[l] = 0;
					if (buffer[l-1] == 3)
						buffer[l-1] = 0;
					__irc_parse_result(server,buffer);
				}
				else //Disconnect? Reconnect!
				{
					spNetCloseTCP(server->connection);
					server->connection = spNetOpenClientTCP(server->ip);
					server->status = 0;
					spNetIRCChannelPointer channel = server->first_channel;
					while (channel)
					{
						__irc_add_message(server,&(channel->first_message),&(channel->last_message),"PRIVMSG","The server disconnected, try to reconnect...",server->name);
						channel = channel->next;
					}

				}
				tcp_thread = NULL;
			}
			spNetIRCChannelPointer mom = server->first_channel;
			while (mom)
			{
				spNetIRCChannelPointer next = mom->next;
				if (mom->close_query)
					__irc_remove_channel(server,mom);
				mom = next;
			}
			if (server->last_add_message != server->last_add_read_message)
			{
				if (server->last_add_read_message == NULL)
					server->last_add_read_message = server->first_add_message;
				else
					server->last_add_read_message = server->last_add_read_message->next;
				while (1)
				{
					__irc_add_message(server,&(server->first_message),&(server->last_message),"PRIVMSG",server->last_add_read_message->message,server->last_add_read_message->user);
					if (server->last_add_read_message->next)
						server->last_add_read_message = server->last_add_read_message->next;
					else
						break;
				}
			}
			mom = server->first_channel;
			while (mom)
			{
				if (mom->last_add_message != mom->last_add_read_message)
				{
					if (mom->last_add_read_message == NULL)
						mom->last_add_read_message = mom->first_add_message;
					else
						mom->last_add_read_message = mom->last_add_read_message->next;
					while (1)
					{
						memcpy(
							__irc_add_message(server,&(mom->first_message),&(mom->last_message),"PRIVMSG",mom->last_add_read_message->message,mom->last_add_read_message->user)->ctcp,
							mom->last_add_read_message->ctcp,
							16);
						if (mom->last_add_read_message->next)
							mom->last_add_read_message = mom->last_add_read_message->next;
						else
							break;
					}
				}
				mom = mom->next;
			}
			if (finish_flag)
			{
				if (tcp_thread)
					SDL_KillThread(tcp_thread);
				break;
			}
		}

		#ifdef REALGP2X
			//TODO: Implement!
			SDL_Delay(100);
		#elif defined _WIN32
			SDL_Delay(100);
		#else
			usleep(100000);
		#endif
	}
	spNetIRCSend(server,"QUIT");
	return 0;
}


PREFIX spNetIRCServerPointer spNetIRCConnectServer(char* name,Uint16 port,char* nickname,char* username,char* realname,char* password)
{
	spNetIRCServerPointer server = (spNetIRCServerPointer)malloc(sizeof(spNetIRCServer));
	server->ip = spNetResolve(name,port);
	if (server->ip.address.ipv4 == SP_INVALID_IP)
	{
		free(server);
		return NULL;
	}
	server->connection = spNetOpenClientTCP(server->ip);
	if (server->connection == NULL)
	{
		free(server);
		return NULL;
	}

	sprintf(server->name,"%s",name);
	sprintf(server->nickname,"%s",nickname);
	sprintf(server->original_nickname,"%s",nickname);
	sprintf(server->username,"%s",username);
	sprintf(server->realname,"%s",realname);
	sprintf(server->password,"%s",password);

	server->counter = 0;
	server->first_message = NULL;
	server->last_message = NULL;
	server->last_read_message = NULL;
	server->first_add_message = NULL;
	server->last_add_message = NULL;
	server->last_add_read_message = NULL;
	server->status = 0;
	server->finish_flag = 0;
	server->first_channel = NULL;
	server->last_channel = NULL;
	server->thread = SDL_CreateThread(__irc_server_thread,server);
	return server;
}

PREFIX spNetIRCChannelPointer spNetIRCJoinChannel(spNetIRCServerPointer server,char* name)
{
	int already;
	spNetIRCChannelPointer channel = __irc_add_channel(server,name,&already);
	if (already)
		return channel;
	if (name[0] != '#')
	{
		channel->status = 1;
		return channel;
	}
	char buffer[1024];
	sprintf(buffer,"JOIN %s",name);
	spNetIRCSend(server,buffer);
	return channel;
}


PREFIX void spNetIRCCloseServer(spNetIRCServerPointer server)
{
	if (server == NULL)
		return;
	server->finish_flag = 1;
	int result;
	SDL_WaitThread(server->thread,&result);
	spNetCloseTCP(server->connection);
	while (server->first_message)
	{
		spNetIRCMessagePointer next = server->first_message->next;
		free(server->first_message);
		server->first_message = next;
	}
	while (server->first_channel)
	{
		while (server->first_channel->first_nick)
		{
			spNetIRCNickPointer next = server->first_channel->first_nick->next;
			free(server->first_channel->first_nick);
			server->first_channel->first_nick = next;
		}
		spNetIRCChannelPointer next = server->first_channel->next;
		free(server->first_channel);
		server->first_channel = next;
	}
	free(server);
}

PREFIX int spNetIRCServerReady(spNetIRCServerPointer server)
{
	return server->status >= 3;
}

PREFIX int spNetIRCChannelReady(spNetIRCChannelPointer channel)
{
	return channel->status;
}

PREFIX void spNetIRCPartChannel(spNetIRCServerPointer server,spNetIRCChannelPointer channel)
{
	//Searching channel in server
	if (channel->name[0] == '#')
	{
		char buffer[512];
		sprintf(buffer,"PART %s",channel->name);
		spNetIRCSend(server,buffer);
	}
	else
		channel->close_query = 1;
}

PREFIX void spNetIRCSendMessage(spNetIRCServerPointer server,spNetIRCChannelPointer channel,char* message)
{
	char buffer[4096];
	if (channel)
	{
		if (message[0] == '/' &&
			message[1] == 'm' &&
			message[2] == 'e' &&
			message[3] == ' ')
		{
			char message_buffer[4096];
			sprintf(message_buffer,"\001ACTION%s\001",&message[3]);
			sprintf(buffer,"PRIVMSG %s :%s",channel->name,message_buffer);
			spNetIRCSend(server,buffer);
			__irc_add_message(server,&(channel->first_add_message),&(channel->last_add_message),"PRIVMSG",message_buffer,server->nickname);
		}
		else
		{
			sprintf(buffer,"PRIVMSG %s :%s",channel->name,message);
			spNetIRCSend(server,buffer);
			__irc_add_message(server,&(channel->first_add_message),&(channel->last_add_message),"PRIVMSG",message,server->nickname);
		}
	}
	else
	{
		sprintf(buffer,"%s",message);
		//uppercase the command
		int i = 0;
		while (buffer[i] != 0 && buffer[i] != ' ')
		{
			buffer[i] = __irc_upper_case(buffer[i]);
			i++;
		}
		__irc_add_message(server,&(server->first_add_message),&(server->last_add_message),"PRIVMSG",message,server->nickname);
		spNetIRCSend(server,buffer);
	}
}

