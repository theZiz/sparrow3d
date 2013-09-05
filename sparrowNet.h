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
#include <time.h>
#include "sparrowFile.h"

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
 * address.ipv4 (Uint32) - 32 bit ipv4 address
 * address.ipv4_bytes (Uint8[4]) - 4 x 8bit ipv4 address
 * address.ipv6 (Uint32[4]) - 128 bit ipv6 address
 * address.ipv6_bytes (Uint8[16]) - 16 x 8bit ipv6 address
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

/* Define: SP_INVALID_IP
 * 
 * Same as INADDR_NONE of SDL_Net. Means, that the ip returned by <spNetResolve>
 * was not found and is invalid. */
#define SP_INVALID_IP INADDR_NONE

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
 * spNetIP - the ip in the type <spNetIP>. If address.ipv4 is SP_INVALID_IP, the
 * function calls failed!*/
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

/* Defines: Compo4all statuses
 * 
 * Statuses for the committing and score-loading functions.
 * 
 * SP_C4A_ESTABLISHING - Connection is being established
 * SP_C4A_PROGRESS - transfer of data is in progress
 * SP_C4A_OK - process is done and everything was fine
 * SP_C4A_ERROR - process is done, but something went wrong */
#define SP_C4A_PROGRESS 1
#define SP_C4A_OK 0
#define SP_C4A_ERROR -1
#define SP_C4A_TIMEOUT -2
#define SP_C4A_CANCELED -3

/* Type: spNetC4AScore
 * 
 * Type for a linked list of scores gotten from the compo4all highscore servers
 * 
 * Variables:
 * longname - full name of the player
 * shortname - three digit short name of the player
 * score - the reached points
 * commitTime - time of type time_t (seconds since 1.1.1970 00:00:00),
 * when the score was committed. Use localtime of time.h to "decrypt"
 * this type. ;)
 * next - pointer to the next element in the list*/
typedef struct spNetC4AScoreStruct *spNetC4AScorePointer;
typedef struct spNetC4AScoreStruct {
	char longname[256];
	char shortname[256];
	int score;
	time_t commitTime;
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
	char password[256];
	char email[256];
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
 * one compo4all background task can run at one time!
 * 
 * Parameters:
 * scoreList - a pointer to spNetC4AProfilePointer, which is in fact a pointer to
 * <spNetC4AScore>. The scores are saved here.
 * profile - an optional pointer to your profile. If given only your scores are
 * added to the scores-list above
 * game - name of the game on the server
 * timeOut - after this time in ms the thread is killed. Get it with
 * <spNetC4AGetTimeOut>
 * 
 * Returns:
 * int - 1 if the function failed for some reason, 0 at success starting
 * the task.
 * 
 * See also: <spNetC4AGetScoreOfMonth>*/
PREFIX int spNetC4AGetScore(spNetC4AScorePointer* scoreList,spNetC4AProfilePointer profile,char* game,int timeOut);

/* Function: spNetC4AGetScoreOfMonth
 * 
 * Loads a top 500 of a month for a given game from the compo4all
 * server. The task runs
 * in background! Use <spNetC4AGetStatus> to get the status of the task. Only
 * one compo4all background task can run at one time!
 * 
 * Parameters:
 * scoreList - a pointer to spNetC4AProfilePointer, which is in fact a pointer to
 * <spNetC4AScore>. The scores are saved here.
 * profile - an optional pointer to your profile. If given only your scores are
 * added to the scores-list above
 * game - name of the game on the server
 * year - year to load (e.g. 2013)
 * month - month to load (e.g. 3 for march)
 * timeOut - after this time in ms the thread is killed. Get it with
 * <spNetC4AGetTimeOut>
 * 
 * Returns:
 * int - 1 if the function failed for some reason, 0 at success starting
 * the task.
 * 
 * See also: <spNetC4AGetScore>*/
PREFIX int spNetC4AGetScoreOfMonth(spNetC4AScorePointer* scoreList,spNetC4AProfilePointer profile,char* game,int year,int month,int timeOut);

/* Function: spNetC4ADeleteScores
 * 
 * Frees the linked list returned by <spNetC4AGetScore>.
 * 
 * Parameters:
 * firstScore - pointer to <spNetC4AScore> to free*/
PREFIX void spNetC4ADeleteScores(spNetC4AScorePointer* scoreList);

/* Function: spNetC4ACommitScore
 * 
 * Commits a score to a specific game to compo4all server. The task runs
 * in background! Use <spNetC4AGetStatus> to get the status of the task. Only
 * one compo4all background task can run at one time!
 * 
 * Parameters:
 * profile - the profile you want to commit tthe score with
 * game - name of the game on the server
 * score - reached score
 * scoreList - pass the struct returned by <spNetC4AGetScore> to compare
 * your score to that list and avoid committing the same score twice. If
 * it is not in the list, it will added afterwards for later
 * comparements.
 * timeOut - after this time in ms the thread is killed. Get it with
 * <spNetC4AGetTimeOut>
 * 
 * Returns:
 * int - 1 if the function failed for some reason (e.g. the score is
 * already in the scoreList), 0 at success starting the task.*/
PREFIX int spNetC4ACommitScore(spNetC4AProfilePointer profile,char* game,int score,spNetC4AScorePointer* scoreList,int timeOut);

/* Function: spNetC4ACreateProfile
 * 
 * Creates a new profile on skeezix' server. Blocks your application! However,
 * if network is avaible, should be quite fast. Furthermore it creates the
 * c4a-prof file needed by all supported applications. The path of the file is
 * platform depended. Overwrites an already existing file! The task runs
 * in background! Use <spNetC4AGetStatus> to get the status of the task. Only
 * one compo4all background task can run at one time!
 * 
 * Parameters:
 * 
 * profile - a pointer to a pointer to <spNetC4AProfile>, where the profile
 * shall be saved to.
 * longname - the long name of the player. Should only be alphanumeric.
 * shortname - the short name of the player. Should exactly 3 alphanumeric,
 * capital letters.
 * password - alphanumeric password
 * email - the mail address of the new account. Can be "".
 * timeOut - after this time in ms the thread is killed. Get it with
 * <spNetC4AGetTimeOut>
 * 
 * Returns:
 * int - 1 if the function failed for some reason, 0 at success starting
 * the task.
 * 
 * See Also:
 * <spNetC4AEditProfile>*/
PREFIX int spNetC4ACreateProfile(spNetC4AProfilePointer* profile, char* longname,char* shortname,char* password,char* email,int timeOut);

/* Function: spNetC4ADeleteAccount
 * 
 * Deletes the given profile on skeezix server and maybe the file on your
 * system. The task runs
 * in background! Use <spNetC4AGetStatus> to get the status of the task. Only
 * one compo4all background task can run at one time!
 * 
 * Parameters:
 * profile - profile to delete from the server.
 * deleteFile - 1 if you want to delete the c4a-prof file at the end
 * (at success), too. 0 leaves to file.
 * timeOut - after this time in ms the thread is killed. Get it with
 * <spNetC4AGetTimeOut>
 * 
 * Returns:
 * int - 1 if the function failed for some reason, 0 at success starting
 * the task.*/
PREFIX int spNetC4ADeleteAccount(spNetC4AProfilePointer* profile,int deleteFile,int timeOut);

/* Function: spNetC4AEditProfile
 * 
 * Edits an already existing profile on skeezix' server. Blocks your
 * application! However, if network is avaible, should be quite fast.
 * Furthermore it rewrites the c4a-prof file needed by all supported
 * applications. The path of the file is platform depended. If the account
 * doesn't exist now, it will created. The big difference to
 * <spNetC4ACreateProfile> is, that the prid is read from the profile struct
 * instead of being new created. The task runs
 * in background! Use <spNetC4AGetStatus> to get the status of the task. Only
 * one compo4all background task can run at one time!
 * 
 * Parameters:
 * 
 * profile - the profile to change.
 * longname - the long name of the player. Should only be alphanumeric.
 * shortname - the short name of the player. Should exactly 3 alphanumeric,
 * capital letters.
 * password - alphanumeric password
 * email - the mail address of the new account. Can be "".
 * timeOut - after this time in ms the thread is killed. Get it with
 * <spNetC4AGetTimeOut>
 * 
 * Returns:
 * int - 1 if the function failed for some reason, 0 at success starting
 * the task.

 * 
 * See Also:
 * <spNetC4ACreateProfile>*/
PREFIX int spNetC4AEditProfile(spNetC4AProfilePointer* profile,char* longname,char* shortname,char* password,char* email,int timeOut);

/* Function: spNetC4ACancelTask
 * 
 * Cancels the C4A task running right now (if one is started).*/
PREFIX void spNetC4ACancelTask();

/* Function: spNetC4AGetTaskResult
 * 
 * Gets the result of the task when finished (check with
 * <spNetC4AGetStatus>.
 * 
 * Returns:
 * int - 0 if everything went fine, 1 at error*/
PREFIX int spNetC4AGetTaskResult();

/* Function: spNetC4AGetTimeOut
 * 
 * Gives you the time out of the C4A tasks.
 * 
 * Returns:
 * int - the timeOut in ms.*/
PREFIX int spNetC4AGetTimeOut();

/* Function: spNetC4AGetStatus
 * 
 * Gets the status of <spNetC4AGetScore>, <spNetC4ACommitScore>,
 * <spNetC4ACreateProfile>, <spNetC4AEditProfile> & <spNetC4ADeleteAccount>.
 * 
 * Returns:
 * int - <Compo4all statuses>*/
PREFIX int spNetC4AGetStatus();

/* Function: spNetC4ADeleteProfileFile
 * 
 * Deletes the profile file on your system, NOT the online account at skeezix
 * server. See also <spNetC4ADeleteAccount>.*/
PREFIX void spNetC4ADeleteProfileFile();

#endif
