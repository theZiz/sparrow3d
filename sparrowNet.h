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

#ifndef _SPARROW_NET_H
#define _SPARROW_NET_H

#include "sparrowDefines.h"
#include <SDL_net.h>

/* File: sparrowNet
 * 
 * SparrowNet / libSparrowNet will be the networking part of sparrow3D.
 * However, at the moment, nothing is implemented.*/

typedef struct spNetC4AScoreStruct *spNetC4AScorePointer;
typedef struct spNetC4AScoreStruct {
	char name[4];
	int points;
	spNetC4AScorePointer next;
} spNetC4AScore;

enum spAddress {IPV4, IPV6};

typedef struct spNetIPStruct *spNetIPPointer;
typedef struct spNetIPStruct {
	enum spAddress type;
	union {
		Uint32 ipv4;
		Uint8 ipv4_bytes[4];
		Uint32 ipv6[4];
		Uint8 ipv6_bytes[16];
	} address;
	Uint16 port;
	IPaddress sdl_address;
} spNetIP;

typedef TCPsocket spNetTCPConnection;
typedef TCPsocket spNetTCPServer;

PREFIX void spInitNet();

PREFIX void spQuitNet();

PREFIX spNetIP spNetResolve(char* host,Uint16 port);

PREFIX char* spNetResolveHost(spNetIP ip,char* host,int host_len);

PREFIX spNetTCPConnection spNetOpenClientTCP(spNetIP ip);

PREFIX spNetTCPServer spNetOpenServerTCP(Uint16 port);

PREFIX spNetTCPConnection spNetAcceptTCP(spNetTCPServer server);

PREFIX spNetIP spNetGetConnectionIP(spNetTCPConnection connection);

PREFIX int spNetSendTCP(spNetTCPConnection connection,void* data,int length);

PREFIX int spNetSendText(spNetTCPConnection connection,char* data);

PREFIX int spNetReceiveTCP(spNetTCPConnection connection,void* data,int length);

PREFIX int spNetReceiveText(spNetTCPConnection connection,char* data,int length);

PREFIX SDL_Thread* spNetReceiveTCPUnblocked(spNetTCPConnection connection,void* data,int length);

PREFIX SDL_Thread* spNetReceiveTextUnblocked(spNetTCPConnection connection,char* data,int length);

PREFIX void spNetCloseTCP(spNetTCPConnection connection);

/* Just definitions! Not working! */
PREFIX char* spNetC4AGetPrid(char* prid,int max_len);

PREFIX spNetC4AScorePointer spNetC4AGetScore(char* prid,char* game);

PREFIX void spNetC4ADeleteScores(spNetC4AScorePointer firstScore);

PREFIX void spNetC4ACommitScore(char* prid,char* game,int score,spNetC4AScorePointer firstScore);

//0 nothing happens, 1 getting IP, 2 establishing connection, 3 sending, 4 closing connection
PREFIX int spNetC4AGetCommitStatus();

#endif
