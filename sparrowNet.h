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

#ifndef _SPARROW_NET_H
#define _SPARROW_NET_H

#include "sparrowDefines.h"
#include <SDL_net.h>
#include <SDL_thread.h>
#include <time.h>

/* File: sparrowNet
 * 
 * SparrowNet is a basic network API based on SDL_net. Until now UDP is not
 * implemented! However, you can use the ip structs from here for the UDP
 * part of SDL_net on your own. ;)
 * 
 * Furthermore sparrowNet has some fancy functions for getting and submitting
 * scores for the compo4all online highscore system and for the IRC chat
 * protocol.*/

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

typedef struct spNetC4ATaskStruct *spNetC4ATaskPointer;
typedef struct spNetC4ATaskStruct {
	SDL_mutex* statusMutex;
	int status;
	void *dataPointer;
	int timeOut;
	SDL_Thread* thread;
	int result;
	int threadStatus;
	int message;
} spNetC4ATask;

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
PREFIX void spInitNet( void );

/* Function: spQuitNet
 * 
 * Quits sparrowNet.*/
PREFIX void spQuitNet( void );

/* Function: spNetResolve
 * 
 * Resolves a ip to a hostname. Only ipv4 is supported at the moment!
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

/* Function: spNetReceiveFinished
 * 
 * Says, whether a receive call (identified by the thread handle) still waits
 * for an incoming signal. Usefull for timeouts. If the thread is finished the
 * number of received bytes is returned.
 * 
 * Parameters:
 * thread - thread of the receiving call, which is maybe still running
 * 
 * Returns:
 * int - 0 if the thread still waits, -1 if the thread is not found and the
 * number of received bytes else.*/
PREFIX int spNetReceiveFinished(SDL_Thread* thread);

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
 * SP_C4A_PROGRESS - transfer of data is in progress
 * SP_C4A_OK - process is done and everything was fine
 * SP_C4A_ERROR - process is done, but something went wrong */
#define SP_C4A_PROGRESS 1
#define SP_C4A_OK 0
#define SP_C4A_ERROR -1
#define SP_C4A_TIMEOUT -2
#define SP_C4A_CANCELED -3

/* Type: spNetC4AGame
 * 
 * Type for a linked list of games from the compo4all highscore servers
 * 
 * Variables:
 * longname - full name of the game
 * shortname - short name of the game
 * status - the status of the highscore: 1 means available, 2 even more
 * available (called "active" by skeezix, don't really know, what this means),
 * 0 inactive, which means, you shouldn't show that game except for testing
 * purpose.
 * genre - the genre of the game.
 * field - which kind of game is it? 0 means just an indie game, 1 is a mame
 * game, -1 means "don't know" or "to be decided".
 * next - pointer to the next element in the list*/
typedef struct spNetC4AGameStruct *spNetC4AGamePointer;
typedef struct spNetC4AGameStruct {
	char longname[256];
	char shortname[256];
	int status;
	char genre[256];
	int field;
	spNetC4AGamePointer next;
} spNetC4AGame;

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
 * next - pointer to the next element in the list
 * rank - rank in the score */
typedef struct spNetC4AScoreStruct *spNetC4AScorePointer;
typedef struct spNetC4AScoreStruct {
	char longname[256];
	char shortname[256];
	int score;
	time_t commitTime;
	spNetC4AScorePointer next;
	int rank;
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
 * 
 * Returns:
 * spNetC4AProfilePointer - a pointer to an <spNetC4AProfile> struct*/
PREFIX spNetC4AProfilePointer spNetC4AGetProfile( void );

/* Function: spNetC4AFreeProfile
 * 
 * Frees the profile you got from <spNetC4AGetProfile>.
 * 
 * Parameters:
 * profile - profile to free. However of course your profile is not deleted. ;)*/
PREFIX void spNetC4AFreeProfile(spNetC4AProfilePointer profile);

/* Function: spNetC4AGetGame
 * 
 * Loads all games listed on the C4A server. Use <spNetC4AGetStatus> to get the
 * status of the task. Only one compo4all background task can run at one time!
 * 
 * Parameters:
 * gameList - a pointer to spNetC4AGamePointer, which is in fact a pointer to
 * <spNetC4AGame>. The available games are saved here.
 * timeOut - after this time in ms the thread is killed. Get it with
 * <spNetC4AGetTimeOut>
 * 
 * Returns:
 * int - 1 if the function failed for some reason, 0 at success starting
 * the task.*/
PREFIX int spNetC4AGetGame(spNetC4AGamePointer* gameList,int timeOut);

/* Function: spNetC4AGetGameParallel
 * 
 * Loads all games listed on the C4A server. Use
 * <spNetC4AGetStatusParallel> to get the status of the task. Don't
 * forget to delete the returned task struct with <spNetC4ADeleteTask>
 * at the end.
 * 
 * Parameters:
 * gameList - a pointer to spNetC4AGamePointer, which is in fact a pointer to
 * <spNetC4AGame>. The available games are saved here.
 * timeOut - after this time in ms the thread is killed. Get it with
 * <spNetC4AGetTimeOut>
 * 
 * Returns:
 * spNetC4ATaskPointer - Pointer to <spNetC4ATask> at success and NULL if an
 * error occured.*/
PREFIX spNetC4ATaskPointer spNetC4AGetGameParallel(spNetC4AGamePointer* gameList,int timeOut);

/* Function: spNetC4ADeleteGames
 * 
 * Frees the linked list returned by <spNetC4AGetGame>.
 * 
 * Parameters:
 * firstGame - pointer to <spNetC4AGame> to free*/
PREFIX void spNetC4ADeleteGames(spNetC4AGamePointer* gameList);

/* Function: spNetC4AGetScore
 * 
 * Loads a top 500 for a given game from the compo4all server. The task runs
 * in background! Use <spNetC4AGetStatus> to get the status of the task. Only
 * one compo4all background task can run at one time!
 * 
 * Parameters:
 * scoreList - a pointer to spNetC4AScorePointer, which is in fact a pointer to
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

/* Function: spNetC4AGetScore
 * 
 * Loads a top 500 for a given game from the compo4all server. The task runs
 * in background! Use <spNetC4AGetStatus> to get the status of the task. Only
 * one compo4all background task can run at one time!
 * 
 * Parameters:
 * scoreList - a pointer to spNetC4AScorePointer, which is in fact a pointer to
 * <spNetC4AScore>. The scores are saved here.
 * profile - an optional pointer to your profile. If given only your scores are
 * added to the scores-list above
 * game - name of the game on the server
 * timeOut - after this time in ms the thread is killed. Get it with
 * <spNetC4AGetTimeOut>
 * 
 * Returns:
 * spNetC4ATaskPointer - Pointer to <spNetC4ATask> at success and NULL if an
 * error occured.
 * 
 * See also: <spNetC4AGetScoreOfMonthParallel>*/
PREFIX spNetC4ATaskPointer spNetC4AGetScoreParallel(spNetC4AScorePointer* scoreList,spNetC4AProfilePointer profile,char* game,int timeOut);

/* Function: spNetC4AFilterScore
 * 
 * Makes, that every name appears only once in the scoreList
 * 
 * Parameter:
 * scoreList - a pointer to spNetC4AScorePointer, which is in fact a pointer to
 * <spNetC4AScore>. These scores are changed.*/
PREFIX void spNetC4AFilterScore(spNetC4AScorePointer* scoreList);

/* Function: spNetC4AGetScoreOfMonth
 * 
 * Loads a top 500 of a month for a given game from the compo4all
 * server. The task runs in background! Use
 * <spNetC4AGetStatusParallel> to get the status of the task. Don't
 * forget to delete the returned task struct with <spNetC4ADeleteTask>
 * at the end.
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
 * spNetC4ATaskPointer - Pointer to <spNetC4ATask> at success and NULL if an
 * error occured.
 * 
 * See also: <spNetC4AGetScoreParallel>*/
PREFIX spNetC4ATaskPointer spNetC4AGetScoreOfMonthParallel(spNetC4AScorePointer* scoreList,spNetC4AProfilePointer profile,char* game,int year,int month,int timeOut);

/* Function: spNetC4CopyScoreList
 * 
 * Makes an unique copy of the passed scorelist, e.g. if you want to remove
 * double names with <spNetC4AMakeScoresUnique> afterwards. Don't forget to
 * free every copy with <spNetC4ADeleteScores> afterwards!
 * 
 * Parameters:
 * scoreList - a pointer to spNetC4AProfilePointer, which is in fact a pointer to
 * <spNetC4AScore>, which shall be copied
 * newList - a pointer to spNetC4AProfilePointer, which is in fact a pointer to
 * <spNetC4AScore>, to which the copy shall be written*/
PREFIX void spNetC4ACopyScoreList(spNetC4AScorePointer* scoreList,spNetC4AScorePointer* newList);

/* Function: spNetC4AMakeScoresUnique
 * 
 * Removes any double occurence of names in the score. Afterwars every
 * name only occurs ones with the best score of this player. Be careful:
 * Afterwards you can't use this list for uploading scores if they don't
 * exist anymore. If you want both make a copy of the list with
 * <spNetC4ACopyScoreList> before (instead of getting the scores twice).
 * 
 * Parameters:
 * scoreList - a pointer to spNetC4AProfilePointer, which is in fact a pointer to
 * <spNetC4AScore> */
PREFIX void spNetC4AMakeScoresUnique(spNetC4AScorePointer* scoreList);

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
 * in background! Use <spNetC4AGetStatusParallel> to get the status of the task.
 * Don't forget to delete the returned task struct with <spNetC4ADeleteTask>
 * at the end.
 * 
 * Parameters:
 * profile - the profile you want to commit tthe score with
 * game - name of the game on the server. If this is empty (""), at least the
 * c4a cache will be tried to commit.
 * score - reached score
 * scoreList - pass the struct returned by <spNetC4AGetScore> to compare
 * your score to that list and avoid committing the same score twice. If
 * it is not in the list, it will added afterwards for later
 * comparements.
 * timeOut - after this time in ms the thread is killed. Get it with
 * <spNetC4AGetTimeOut>. If timeOut is 0, no thread is started, but the
 * score is written to the cache if enabled!
 * 
 * Returns:
 * int - 1 if the function failed for some reason (e.g. the score is
 * already in the scoreList), 0 at success starting the task.*/
PREFIX int spNetC4ACommitScore(spNetC4AProfilePointer profile,char* game,int score,spNetC4AScorePointer* scoreList,int timeOut);

/* Function: spNetC4ACommitScoreParallel
 * 
 * Commits a score to a specific game to compo4all server. The task runs
 * in background! Use <spNetC4AGetStatusParallel> to get the status of the task.
 * Don't forget to delete the returned task struct with <spNetC4ADeleteTask>
 * at the end.
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
 * <spNetC4AGetTimeOut> If timeOut is 0, no thread is started, but the
 * score is written to the cache if enabled!
 * 
 * Returns:
 * spNetC4ATaskPointer - Pointer to <spNetC4ATask> at success and NULL if an
 * error occured.*/
PREFIX spNetC4ATaskPointer spNetC4ACommitScoreParallel(spNetC4AProfilePointer profile,char* game,int score,spNetC4AScorePointer* scoreList,int timeOut);

/* Function: spNetC4ACreateProfile
 * 
 * Creates a new profile on skeezix' server. Blocks your application! However,
 * if network is avaible, should be quite fast. Furthermore it creates the
 * c4a-prof file needed by all supported applications. The path of the file is
 * platform depended. Overwrites an already existing file! The task runs
 * in background! Use <spNetC4AGetStatus> to get the status of the task. Only
 * one compo4all background task can run at one time!
 * Only for Pandora: If no c4a-prof is created until now and no c4a-mame appdata
 * folder is available it will be created in the first folder in /media with
 * pandora/appdata in it. It MAY be, that skeezix's C4A Manager will not find
 * this c4a-prof if it will be installed later on another SD card! However the
 * spaghetti client, the fusilli client and the Sparrow C4A Manager will find
 * this profile file in any case (if you don't create a second c4a-prof with
 * skeezix c4a manager. It that case it is random which c4a-prof is used in the
 * end...)
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
PREFIX void spNetC4ACancelTask( void );

/* Function: spNetC4ACancelTaskParallel
 * 
 * Cancels the given C4A task. Don't forget to delete it afterwards with
 * <spNetC4ADeleteTask>!
 * 
 * Parameters:
 * task - pointer to <spNetC4ATask> returned from the parallel functions.*/
PREFIX void spNetC4ACancelTaskParallel(spNetC4ATaskPointer task);

/* Function: spNetC4AGetTaskResult
 * 
 * Gets the result of the task when finished (check with
 * <spNetC4AGetStatus>).
 * 
 * Returns:
 * int - 0 if everything went fine, 1 at error*/
PREFIX int spNetC4AGetTaskResult( void );

/* Function: spNetC4AGetTaskResultParallel
 * 
 * Gets the result of the given task when finished (check with
 * <spNetC4AGetStatusParallel>).
 * 
 * Parameters:
 * task - pointer to <spNetC4ATask> returned from the parallel functions.
 * 
 * Returns:
 * int - 0 if everything went fine, 1 at error*/
PREFIX int spNetC4AGetTaskResultParallel(spNetC4ATaskPointer task);

/* Function: spNetC4AGetTimeOut
 * 
 * Gives you the time out of the C4A tasks.
 * 
 * Returns:
 * int - the timeOut in ms.*/
PREFIX int spNetC4AGetTimeOut( void );

/* Function: spNetC4AGetTimeOutParallel
 * 
 * Gives you the time out of the given C4A task.
 * 
 * Parameters:
 * task - pointer to <spNetC4ATask> returned from the parallel functions.
 * 
 * Returns:
 * int - the timeOut in ms.*/
PREFIX int spNetC4AGetTimeOutParallel(spNetC4ATaskPointer task);

/* Function: spNetC4AGetStatus
 * 
 * Gets the status of <spNetC4AGetScore>, <spNetC4ACommitScore>,
 * <spNetC4ACreateProfile>, <spNetC4AEditProfile> & <spNetC4ADeleteAccount>.
 * 
 * Returns:
 * int - <Compo4all statuses>*/
PREFIX int spNetC4AGetStatus( void );

/* Function: spNetC4AGetStatusParallel
 * 
 * Gets the status of <spNetC4AGetScoreParallel> & <spNetC4ACommitScoreParallel>.
 * 
 * Parameters:
 * task - pointer to <spNetC4ATask> returned from the parallel functions
 * 
 * Returns:
 * int - <Compo4all statuses>*/
PREFIX int spNetC4AGetStatusParallel(spNetC4ATaskPointer task);


/* Function: spNetC4ADeleteProfileFile
 * 
 * Deletes the profile file on your system, NOT the online account at skeezix
 * server. See also <spNetC4ADeleteAccount>.*/
PREFIX void spNetC4ADeleteProfileFile( void );

/* Function: spNetC4ADeleteTask
 * 
 * Deletes the task struct returned by the parallel working task functions.
 * 
 * Parameters:
 * task - Pointer to <spNetC4ATask>, which is returned by the parallel functions.*/
PREFIX void spNetC4ADeleteTask(spNetC4ATaskPointer task);

/* Function: spNetC4ASetCaching
 * 
 * (De)Activates caching for C4A scores. If caching is enabled every time
 * spNetC4ACommitScore* fails, it is written to a cache file and the next time
 * spNetC4ACommitScore* is called it will be tried to be committed again
 * 
 * Parameters:
 * value - 0: No caching
 * 1: Everything is cached
 * 2: Only the best score of a game is cached
 * 3: Same like 2, but lower score is better*/
PREFIX void spNetC4ASetCaching(int value);

/* Function: spNetC4AHowManyCached
 * 
 * Returns how many scores are cached
 * 
 * Returns:
 * int - the number of cached scores*/
PREFIX int spNetC4AHowManyCached( void );

/* Section: IRC chat protocol stuff
 * 
 * Types and Functions for connecting to an IRC server, join channels and
 * receive and send message.*/

/* Type: spNetIRCMessage
 * 
 * Type for a linked list of IRC messages managed from the IRC server. 
 * 
 * Variables:
 * type - type of the message (not always set)
 * user - user. * means server
 * message - message of the message
 * next - pointer to the next element in the list
 * ctcp - char[16], if not "" the name of the ctcp event like ACTION.*/
typedef struct spNetIRCMessageStruct *spNetIRCMessagePointer;
typedef struct spNetIRCMessageStruct {
	char type[128];
	char user[128];
	char message[1024];
	time_t time_stamp;
	spNetIRCMessagePointer next;
	char ctcp[16];
} spNetIRCMessage;

/* Type: spNetIRCNick
 * 
 * Type for a linked list of IRC nicks managed from the IRC server.
 * 
 * Variables:
 * name - name of the nick
 * rights - rights of the nick, e.g. ' ', @ or +
 * next - pointer to the next element in the list*/
typedef struct spNetIRCNickStruct *spNetIRCNickPointer;
typedef struct spNetIRCNickStruct {
	char name[256];
	char rights;
	spNetIRCNickPointer next;
} spNetIRCNick;

/* Type: spNetIRCChannel
 * 
 * Type for a linked list of IRC channels managed from the IRC server. With
 * channel are meant channels from the type #meow and queries!
 * 
 * Variables:
 * name - name of the channel
 * status - internal status flag of the channel
 * first_nick - pointer to <spNetIRCNick>, first nick in the channel
 * last_nick - pointer to <spNetIRCNick>, last nick in the channel
 * next - pointer to the next element in the list
 * first_message - pointer to <spNetIRCMessage>, first message in the channel
 * last_message - pointer to <spNetIRCMessage>, last message in the channel
 * last_read_message - pointer to <spNetIRCMessage>, last read message. You have
 * to update this yourself!
 * close_query - internal message flag
 * first_add_message - pointer to <spNetIRCMessage>, internal chain
 * last_add_message - pointer to <spNetIRCMessage>, internal chain
 * last_add_read_message - pointer to <spNetIRCMessage>, internal chain*/ 
typedef struct spNetIRCChannelStruct *spNetIRCChannelPointer;
typedef struct spNetIRCChannelStruct {
	char name[256];
	int status;
	spNetIRCNickPointer first_nick;
	int got_end_366;
	spNetIRCChannelPointer next;
	spNetIRCMessagePointer first_message;
	spNetIRCMessagePointer last_message;
	spNetIRCMessagePointer last_read_message;
	int close_query;
	spNetIRCMessagePointer first_add_message;
	spNetIRCMessagePointer last_add_message;
	spNetIRCMessagePointer last_add_read_message;
	int show_users;
} spNetIRCChannel;

/* Type: spNetIRCServer
 * 
 * Handle type for interaction with the irc server. You should not use it
 * direct but with the spNetIRC* functions.
 * 
 * Variables:
 * name - name/url of the server
 * port - port of the server (most of the time 6667)
 * nickname - nickname of the chatter
 * realname - the REAL name of the chatter!
 * firstChannel - pointer the first element of a list of all channels
 * ip - <spNetIP> struct. IP of the server.
 * connection - <spNetTCPConnection> struct. connection handle to the server
 * thread - SDL thread struct
 * finish_flag - messages for the server to finish
 * status - internal status flag
 * counter - internal nickname counter
 * original_nickname - internal nickname to copy from
 * first_message - pointer to <spNetIRCMessage>, first message of the server
 * last_message - pointer to <spNetIRCMessage>, last message of the server
 * last_read_message - pointer to <spNetIRCMessage>, last read message. You have
 * to update this yourself!
 * first_add_message - pointer to <spNetIRCMessage>, internal chain
 * last_add_message - pointer to <spNetIRCMessage>, internal chain
 * last_add_read_message - pointer to <spNetIRCMessage>, internal chain*/ 
typedef struct spNetIRCServerStruct *spNetIRCServerPointer;
typedef struct spNetIRCServerStruct {
	char name[256];
	Uint16 port;
	char nickname[256];
	char username[256];
	char realname[256];
	char password[256];
	spNetIRCChannelPointer first_channel;
	spNetIRCChannelPointer last_channel;
	spNetIP ip;
	spNetTCPConnection connection;
	SDL_Thread* thread;
	int finish_flag;
	int status;
	int counter;
	char original_nickname[256];
	spNetIRCMessagePointer first_message;
	spNetIRCMessagePointer last_message;
	spNetIRCMessagePointer last_read_message;
	spNetIRCMessagePointer first_add_message;
	spNetIRCMessagePointer last_add_message;
	spNetIRCMessagePointer last_add_read_message;
} spNetIRCServer;

/* Function: spNetIRCConnectServer
 * 
 * Connects to an IRC server. If the attempt was sucessful a new thread is
 * created and a pointer to <spNetIRCServer> returned.
 * 
 * Parameters:
 * name - name/url of the server
 * port - port of the server (e.g. 6667)
 * nickname - nickname to use. If the nickname is already used, it will be used
 * another one with numbers and stuff
 * username - your user name
 * realname - your real name
 * password - password
 * 
 * Returns:
 * spNetIRCServerPointer - a pointer to <spNetIRCServer> to use in further
 * spNetIRC* functions*/
PREFIX spNetIRCServerPointer spNetIRCConnectServer(char* name,Uint16 port,char* nickname,char* username,char* realname,char* password);

/* Function: spNetIRCServerReady
 * 
 * Returns, whether the server is ready to be used, e.g. for joining channels
 * 
 * Parameters:
 * server - pointer to <spNetIRCServer> struct
 * 
 * Returns:
 * int - 1: the server is ready; 0: not ready. If you try to join a channel on a 
 * not ready server, it may not work!*/
PREFIX int spNetIRCServerReady(spNetIRCServerPointer server);

/* Function: spNetIRCJoinChannel
 * 
 * Joins an IRC channel. Call it only if <spNetIRCReady> returns 1!
 * 
 * Parameters:
 * server - pointer to <spNetIRCServer> struct
 * name - name of the channel. Has to start with #!
 * 
 * Returns:
 * spNetIRCChannelPointer - Pointer to <spNetIRCServerReady> struct, handle for
 * the channel*/
PREFIX spNetIRCChannelPointer spNetIRCJoinChannel(spNetIRCServerPointer server,char* name);

/* Function: spNetIRCChannelReady
 * 
 * Returns, whether the channel is ready to be used, e.g. for sending messages
 * 
 * Parameters:
 * channel - pointer to <spNetIRCChannel> struct
 * 
 * Returns:
 * int - 1: the channel is ready; 0: not ready. -1: an error occured, e.g. you
 * are banned*/
PREFIX int spNetIRCChannelReady(spNetIRCChannelPointer channel);

/* Function: spNetIRCSendMessage
 * 
 * Sends a message (PRIVMSG) to a channel or another user. If channel is NULL
 * it will be send to the server instead (e.g.
 * spNetIRCSendMessage(server,NULL,"quit") for /quit).
 * 
 * Parameters:
 * server - pointer to <spNetIRCServer> struct
 * channel - pointer to <spNetIRCChannel> struct, may be NULL
 * message - message to send*/
PREFIX void spNetIRCSendMessage(spNetIRCServerPointer server,spNetIRCChannelPointer channel,char* message);

/* Function: spNetIRCPartChannel
 * 
 * Leave a channel / conversation. You should not use the channel pointer after
 * this point. It may need some time for the PART command to get to the server
 * but at one point the pointer will be invalid! It will removed from server
 * then automaticly.
 * 
 * Parameters:
 * spNetIRCServerPointer - a pointer to <spNetIRCServer>
 * channel - pointer to <spNetIRCChannel> struct*/
PREFIX void spNetIRCPartChannel(spNetIRCServerPointer server,spNetIRCChannelPointer channel);

/* Function: spNetIRCCloseServer
 * 
 * Closes the connection to the server, finishs the thread and releases the
 * pointer handle!
 * 
 * Parameters:
 * spNetIRCServerPointer - a pointer to <spNetIRCServer>, which describes which
 * server shall be exited*/
PREFIX void spNetIRCCloseServer(spNetIRCServerPointer server);


#endif
