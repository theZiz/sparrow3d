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

PREFIX int spNetSendText(spNetTCPConnection connection,char* data)
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
	int res=SDLNet_TCP_Recv(tcpData->connection,tcpData->data,tcpData->length);
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
	return SDLNet_TCP_Recv(connection,data,length);
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

PREFIX int spNetReceiveText(spNetTCPConnection connection,char* data,int length)
{
	return spNetReceiveTCP(connection,data,length);
}

PREFIX SDL_Thread* spNetReceiveTextUnblocked(spNetTCPConnection connection,char* data,int length)
{
	return spNetReceiveTCPUnblocked(connection,data,length);
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

int spNetC4AStatus = 0;
typedef struct getscoreStruct *getscorePointer;
typedef struct getscoreStruct {
	spNetC4AScorePointer* score;
	spNetC4AProfile profile;
	char game[256];
} getscoreType;


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
	sprintf(get_string,"GET /json_1/%s\n\n",scoreData->game);
	if (spNetSendText(connection,get_string) == 0)
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
	if ((length = spNetReceiveText(connection,buffer,50000)) == 0)
	{
		spNetCloseTCP(connection);
		free(data);
		SDL_mutexP(spNetC4AStatusMutex);
		spNetC4AStatus = SP_C4A_ERROR;
		SDL_mutexV(spNetC4AStatusMutex);
		return 1;
	}
	buffer[length] = 0;
	printf("%s\n",buffer);
	spNetCloseTCP(connection);
	free(data);
	SDL_mutexP(spNetC4AStatusMutex);
	spNetC4AStatus = SP_C4A_OK;
	SDL_mutexV(spNetC4AStatusMutex);
	return 0;
}

PREFIX void spNetC4AGetScore(spNetC4AScorePointer* score,spNetC4AProfile profile,char* game)
{
	(*score) = NULL;
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	if (status == SP_C4A_OK || status == SP_C4A_ERROR)
	{
		//Starting a background thread, which does the fancy stuff
		getscorePointer data = (getscorePointer)malloc(sizeof(getscoreType));
		data->score = score;
		data->profile = profile;
		sprintf(data->game,"%s",game);
		SDL_CreateThread(c4a_getscore_thread,data);
	}
}


PREFIX int spNetC4AGetStatus()
{
	SDL_mutexP(spNetC4AStatusMutex);
	int status = spNetC4AStatus;
	SDL_mutexV(spNetC4AStatusMutex);
	return status;
}
