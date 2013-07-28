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
#include <SDL_thread.h>

/* File: sparrowNet
 * 
 * SparrowNet is a basic network API based on SDL_net. Until now UDP is not
 * implemented! However, you can use the ip structs from here for the UDP
 * part of SDL_net on your own. ;)
 * 
 * Furthermore sparrowNet has some fancy functions for getting and submitting
 * scores for the compo4all online highscore system.*/

enum spAddress {IPV4, IPV6};

/* Section: General & TCP Stuff
 * 
 * Types and Functions for initializing sparrowNet and sending data over tcp.*/

/* Type: spNetIP
 * 
 * An IP address resolved by sparrowNet.
 * 
 * Variables:
 * ipv4 (Uint32) - 32 bit ipv4 address
 * ipv4_bytes (Uint8[4]) - 4 x 8bit ipv4 address
 * ipv6 (Uint32[4]) - 128 bit ipv6 address
 * ipv6_bytes (Uint8[16]) - 16 x 8bit ipv6 address
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

/* Function: spNetSendHTTP
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
PREFIX int spNetSendHTTP(spNetTCPConnection connection,char* data);

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

/* Function: spNetReceiveHTTP
 * 
 * Receives data from the other side until the other side disconnects. This
 * function blocks your application until the client actually sends something!
 * You can of course use other protocols than http. ;)
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
PREFIX int spNetReceiveHTTP(spNetTCPConnection connection,char* data,int length);

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

/* Function: spNetReceiveHTTPUnblocked
 * 
 * Receives data from the other side until the other side disconnects. This
 * function does not block your application! However, it may need a while until
 * your counterpart sends something and your data string stays invalid. You
 * can of course use other protocols than http. ;)
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
PREFIX SDL_Thread* spNetReceiveHTTPUnblocked(spNetTCPConnection connection,char* data,int length);

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

/* Section: Compo4all stuff
 * 
 * Types and Functions for sending and receiving data to and from the compo4all
 * server.*/

/* Type: spNetC4AScore
 * 
 * Type for a linked list of scores gotten from the compo4all highscore servers
 * 
 * Variables:
 * longname - full name of the player
 * shortname - three digit short name of the player
 * score - the reached points
 * next - pointer to the next element in the list*/
typedef struct spNetC4AScoreStruct *spNetC4AScorePointer;
typedef struct spNetC4AScoreStruct {
	char longname[256];
	char shortname[256];
	int score;
	spNetC4AScorePointer next;
} spNetC4AScore;

/* Type: spNetC4AProfile
 * 
 * A struct for your *unique* C4A Profile.
 * 
 * Variables:
 * prid - unique id of your account. Keep it for you. ;)
 * longname - your full name
 * shortname - your 3 digit short name */
typedef struct spNetC4AProfileStruct *spNetC4AProfilePointer;
typedef struct spNetC4AProfileStruct {
	char prid[256];
	char longname[256];
	char shortname[256];
} spNetC4AProfile;


/* Function: spNetC4AGetProfile
 * 
 * Reads your profile out of your c4a-prof file. On the pandora the file is
 * created by compo4all and stored on a pandora specific place. On other systems
 * for now your c4a-prof should be in the folder of your application.
 * Unfortunately it is quite hard for a not pandora owner to get such a
 * compo4all account. But a webserver for this purpose is in the making. :)
 * 
 * Returns:
 * spNetC4AProfilePointer - a pointer to an <spNetC4AProfile> struct*/
PREFIX spNetC4AProfilePointer spNetC4AGetProfile();

/* Function: spNetC4AFreeProfile
 * 
 * Frees the profile you got from <spNetC4AGetProfile>.
 * 
 * Parameters:
 * profile - profile to free. However of course your profile is not deleted. ;)*/
PREFIX void spNetC4AFreeProfile(spNetC4AProfilePointer profile);

/* Function: spNetC4AGetScore
 * 
 * Loads a top 500 for a given game from the compo4all server. The task runs
 * in background! Use <spNetC4AGetStatus> to get the status of the task. Only
 * one background task (fetching highscore OR commiting a score) can run at one
 * time!
 * 
 * Parameters:
 * score - a pointer to spNetC4AProfilePointer, which is in fact a pointer to
 * <spNetC4AScore>
 * profile - an optional pointer to your profile. If given only your scores are
 * added to the scores-list above
 * game - name of the game on the server
 * 
 * Returns:
 * SDL_Thread* - handle to the created thread. If you don't want to run the
 * task in background you can e.g. call SDL_WaitThread with this return value or
 * maybe kill it after a timeout.
 * 
 * See also: <spNetC4AGetScoreOfMonth>*/
PREFIX SDL_Thread* spNetC4AGetScore(spNetC4AScorePointer* score,spNetC4AProfilePointer profile,char* game);

/* Function: spNetC4AGetScoreOfMonth
 * 
 * Loads a top 500 of a month for a given game from the compo4all
 * server. The task runs in background! Use <spNetC4AGetStatus> to get
 * the status of the task. Only one background task (fetching highscore
 * OR commiting a score) can run at one time!
 * 
 * Parameters:
 * score - a pointer to spNetC4AProfilePointer, which is in fact a pointer to
 * <spNetC4AScore>
 * profile - an optional pointer to your profile. If given only your scores are
 * added to the scores-list above
 * game - name of the game on the server
 * year - year to load (e.g. 2013)
 * month - month to load (e.g. 3 for march)
 * 
 * Returns:
 * SDL_Thread* - handle to the created thread. If you don't want to run the
 * task in background you can e.g. call SDL_WaitThread with this return value or
 * maybe kill it after a timeout.
 * 
 * See also: <spNetC4AGetScore>*/
PREFIX SDL_Thread* spNetC4AGetScoreOfMonth(spNetC4AScorePointer* score,spNetC4AProfilePointer profile,char* game,int year,int month);

/* Function: spNetC4ADeleteScores
 * 
 * Frees the linked list returned by <spNetC4AGetScore>.
 * 
 * Parameters:
 * firstScore - pointer to <spNetC4AScore> to free*/
PREFIX void spNetC4ADeleteScores(spNetC4AScorePointer* firstScore);

/* Function: spNetC4ACommitScore
 * 
 * Commits a score to a specific game to compo4all server. The task runs
 * in background! Use <spNetC4AGetStatus> to get the status of the task. Only
 * one background task (fetching highscore OR commiting a score) can run at one
 * time!
 * 
 * Parameters:
 * profile - the profile you want to commit tthe score with
 * game - name of the game on the server
 * score - reached score
 * firstScore - pointer to a <spNetC4AScore> struct. If given it is first
 * checked, whether you already have this score. ;) So you can commit always
 * ALL scores of your game and wont have any doubles. "Caching" for free!
 * 
 * Returns:
 * SDL_Thread* - handle to the created thread. If you don't want to run the
 * task in background you can e.g. call SDL_WaitThread with this return value or
 * maybe kill it after a timeout.*/
PREFIX SDL_Thread* spNetC4ACommitScore(spNetC4AProfilePointer profile,char* game,int score,spNetC4AScorePointer* firstScore);


/* Defines: Compo4all statuses
 * 
 * Statuses for the committing and score-loading functions.
 * 
 * SP_C4A_ESTABLISHING - Connection is being established
 * SP_C4A_PROGRESS - transfer of data is in progress
 * SP_C4A_OK - process is done and everything was fine
 * SP_C4A_ERROR - process is done, but something went wrong */
#define SP_C4A_ESTABLISHING 2
#define SP_C4A_PROGRESS 1
#define SP_C4A_OK 0
#define SP_C4A_ERROR -1

/* Function: spNetC4AGetStatus
 * 
 * Gets the status of <spNetC4AGetScore> and <spNetC4ACommitScore>.
 * 
 * Returns:
 * int - <Compo4all statuses>*/
PREFIX int spNetC4AGetStatus();

#endif
