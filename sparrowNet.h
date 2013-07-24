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
 * SparrowNet is a basic network API based on SDL_net. Until now UDP is not
 * implemented! However, you can use the ip structs from here for the UDP
 * part of SDL_net on your own. ;)*/

typedef struct spNetC4AScoreStruct *spNetC4AScorePointer;
typedef struct spNetC4AScoreStruct {
	char name[4];
	int points;
	spNetC4AScorePointer next;
} spNetC4AScore;

enum spAddress {IPV4, IPV6};

/* Type: spNetIP
 * 
 * An IP address resolved by sparrowNet.
 * 
 * Variables:
 * ipv4 (Uint32) - 32 bit ipv4 address
 * ipv4_bytes (Uint8[4]) - 4 x 8bit ipv4 address
 * ipv6 (Uint32[4]) - 128 bit ipv4 address
 * ipv6_bytes (Uint8[16]) - 16 x 8bit ipv4 address
 * port (Uint16) - port of the address
 * sdl_address (IPaddress) - ip address struct of SDL*/
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

/* Type: spNetTCPConnection 
 * 
 * a type for open tcp connections. In fact just SDL_net's TCPsocket.*/
typedef TCPsocket spNetTCPConnection;
/* Type: spNetTCPServer
 * 
 * a type for server sessions. In fact just SDL_net's TCPsocket.*/
typedef TCPsocket spNetTCPServer;

/* Function: spInitNet
 * 
 * Initializes sparrowNet. Call always before using this library!*/
PREFIX void spInitNet();

/* Function: spQuitNet
 * 
 * Quits sparrowNet.*/
PREFIX void spQuitNet();

/* Function: spNetResolve
 * 
 * Resolves a hostname to an ip. Only ipv4 is supported at the moment!
 * 
 * Parameters:
 * host - the host name
 * port - the port you want to access (later)
 * 
 * Returns:
 * spNetIP - the ip in the type <spNetIP>*/
PREFIX spNetIP spNetResolve(char* host,Uint16 port);

/* Function: spNetResolveHost
 * 
 * Gives you the hostname to an ip.
 * 
 * Parameters:
 * ip - ip the the host you want to know
 * host - a pointer, where to put the string
 * host_len - the maximal length of the host string. Keep the ending 0 in mind!
 * 
 * Returns:
 * char* - the pointer "host" from above*/
PREFIX char* spNetResolveHost(spNetIP ip,char* host,int host_len);

/* Function: spNetOpenClientTCP
 * 
 * Opens a connection to an ip
 * 
 * Parameters:
 * ip - the ip
 * 
 * Returns:
 * spNetTCPConnection - handle for the connection */
PREFIX spNetTCPConnection spNetOpenClientTCP(spNetIP ip); 

/* Function: spNetOpenServerTCP
 * 
 * Creates a server, which listens at a port
 * 
 * Parameters:
 * port - the port
 * 
 * Returns:
 * spNetTCPServer - handle for the server*/
PREFIX spNetTCPServer spNetOpenServerTCP(Uint16 port);

/* Function: spNetAcceptTCP
 * 
 * If a client tries to connect to your server, this function returns an
 * established tcp connection from your server to the client.
 * 
 * Parameters:
 * server - the server returned by <spNetOpenServerTCP>*/
PREFIX spNetTCPConnection spNetAcceptTCP(spNetTCPServer server);

/* Function: spNetGetConnectionIP
 * 
 * Gives you the ip of your connected client.
 * 
 * Parameters:
 * connection - a connection, where you want to know other side
 * 
 * Returns
 * spNetIP - the ip as <spNetIP>*/
PREFIX spNetIP spNetGetConnectionIP(spNetTCPConnection connection);

/* Function: spNetSendTCP
 * 
 * Sends a tcp package to the ether.
 * 
 * Parameters:
 * connection - the connection to send a tcp package with
 * data - data to send. Can be a pointer to literally anything.
 * length - length of the data to be sent (in bytes)
 * 
 * Returns:
 * int - the number of sent bytes. If unequal length the connection seem to be
 * closed */
PREFIX int spNetSendTCP(spNetTCPConnection connection,void* data,int length);

/* Function: spNetSendText
 * 
 * Sends text over TCP with spNetSendTCP.
 * 
 * Parameters:
 * connection - the connection to send a tcp package with
 * data - the string to send
 * 
 * Returns:
 * int - the number of sent bytes. If unequal length the connection seem to be
 * closed */
PREFIX int spNetSendText(spNetTCPConnection connection,char* data);

/* Function: spNetReceiveTCP
 * 
 * Receives data from the other side. This function blocks your application
 * until the client actually sends something!
 * 
 * Parameters:
 * connection - the connection you want to receive tcp packages from
 * data - data to receive. Can be a pointer to literally anything.
 * length - maximal data to receive
 * 
 * Returns:
 * int - the number of received bytes. If 0 something strange happend. E.g. your
 * counterside explodes or just closed the connection. However your connection
 * is invalid now! Better close, too. */
PREFIX int spNetReceiveTCP(spNetTCPConnection connection,void* data,int length);

/* Function: spNetReceiveText
 * 
 * Receives text from the other side. This function blocks your application
 * until the client actually sends something!
 * 
 * Parameters:
 * connection - the connection you want to receive tcp packages from
 * data - pointer to the text to receive
 * length - maximal length of the text
 * 
 * Returns:
 * int - the number of received characters. If 0 something strange happend. E.g.
 * your counterside explodes or just closed the connection. However your
 * connection is invalid now! Better close, too. */
PREFIX int spNetReceiveText(spNetTCPConnection connection,char* data,int length);

/* Function: spNetReceiveTCPUnblocked
 * 
 * Receives data from the other side. This function does not block your
 * application! However, it may need a while until your counterpart sends
 * something and your data string stays invalid.
 * 
 * Parameters:
 * connection - the connection you want to receive tcp packages from
 * data - data to receive. Can be a pointer to literally anything.
 * length - maximal data to receive
 * 
 * Returns:
 * SDL_Thread* - a handle to the created background thread. E.g. you can kill
 * it with SDL_KillThread if you don't want to wait anymore. If (void*)(-1) is
 * returned the connection is closed (or your counterside exploded).*/
PREFIX SDL_Thread* spNetReceiveTCPUnblocked(spNetTCPConnection connection,void* data,int length);

/* Function: spNetReceiveTextUnblocked
 * 
 * Receives text from the other side. This function does not block your
 * application! However, it may need a while until your counterpart sends
 * something and your data string stays invalid.
 * 
 * Parameters:
 * 
 * connection - the connection you want to receive tcp packages from
 * data - pointer to the text to receive
 * length - maximal length of the text
 * 
 * Returns:
 * SDL_Thread* - a handle to the created background thread. E.g. you can kill
 * it with SDL_KillThread if you don't want to wait anymore. If (void*)(-1) is
 * returned the connection is closed (or your counterside exploded).*/
PREFIX SDL_Thread* spNetReceiveTextUnblocked(spNetTCPConnection connection,char* data,int length);

/* Function: spNetReceiveStillWaiting
 * 
 * Says, whether a receive call (identified by the thread handle) still waits
 * for an incoming signal. Usefull for timeouts.
 * 
 * Parameters:
 * thread - thread of the receiving call, which is maybe still running
 * 
 * Returns:
 * int - 1 if the thread still waits or 0 if not.*/
PREFIX int spNetReceiveStillWaiting(SDL_Thread* thread);

/* Function: spNetCloseTCP
 * 
 * Closes a tcp connection or a server session.
 * 
 * Parameter:
 * connection - connection to close*/
PREFIX void spNetCloseTCP(spNetTCPConnection connection);

/* Just definitions! Not working! */
PREFIX char* spNetC4AGetPrid(char* prid,int max_len);

PREFIX spNetC4AScorePointer spNetC4AGetScore(char* prid,char* game);

PREFIX void spNetC4ADeleteScores(spNetC4AScorePointer firstScore);

PREFIX void spNetC4ACommitScore(char* prid,char* game,int score,spNetC4AScorePointer firstScore);

//0 nothing happens, 1 getting IP, 2 establishing connection, 3 sending, 4 closing connection
PREFIX int spNetC4AGetCommitStatus();

#endif
