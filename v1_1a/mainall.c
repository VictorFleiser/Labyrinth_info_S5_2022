//client.c
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <unistd.h>
#include <stdarg.h>



#define MAX_GET_MOVE 128	    	/* maximum size of the string representing a move */
#define MAX_MESSAGE 1024			/* maximum size of the message move */




//client.h


/* defines a return code, used for playMove and getMove */
typedef enum
{
	NORMAL_MOVE = 0,
	WINNING_MOVE = 1,
	LOOSING_MOVE = -1
} t_return_code;



//client.c

#define h_addr h_addr_list[0] /* for backward compatibility */

#define HEAD_SIZE 6 			/*number of bytes to code the size of the message (header)*/
#define MAX_LENGTH 20000 		/* maximum size of the buffer expected for print_Game */

/* global variables about the connection
 * we use them just to hide all the connection details to the user
 * so no need to know about them, or give them when we use the functions of this API
*/
int sockfd = -1;		        /* socket descriptor, equal to -1 when we are not yet connected */
char buffer[MAX_LENGTH];		/* global buffer used to send message (global so that it is not allocated/desallocated for each message) */
int debug=0;			        /* debug constant; we do not use here a #DEFINE, since it allows the client to declare 'extern int debug;' set it to 1 to have debug information, without having to re-compile labyrinthAPI.c */
char playerName[21] = {};            /* name of the player, stored to display it in debug */


/* Display Error message and exit
 *
 * Parameters:
 * - fct: name of the function where the error raises (__FUNCTION__ can be used)
 * - msg: message to display
 * - ...: extra parameters to give to printf...
*/
void dispError(const char* fct, const char* msg, ...)
{
	va_list args;
	va_start (args, msg);
	fprintf( stderr, "\e[5m\e[31m\u2327\e[2m [%s] (%s)\e[0m ", playerName, fct);
	vfprintf( stderr, msg, args);
	fprintf( stderr, "\n");
	va_end (args);
	exit(EXIT_FAILURE);
}


/* Display Debug message (only if `debug` constant is set to 1)
 *
 * Parameters:
 * - fct: name of the function where the error raises (__FUNCTION__ can be used)
 * - level : debug level (print if debug>=level, level=0 always print)
 * - msg: message to display
 * - ...: extra parameters to give to printf...
*/
void dispDebug(const char* fct, int level, const char* msg, ...)
{
  if (debug>=level)
	{
		printf("\e[35m\u26A0\e[0m [%s] (%s) ", playerName, fct);

		/* print the msg, using the varying number of parameters */
		va_list args;
		va_start (args, msg);
		vprintf(msg, args);
		va_end (args);

		printf("\n");
	}
}

/* Read the message and fill the buffer
* Parameters:
* - fct : name of the calling function
* - buf: pointer to the buffer variable (already allocated)
* - nbuf : size of the buffer
*
* Return the remaining length of the message (0 is the message is completely read)
*/
size_t read_inbuf(const char *fct, char *buf, size_t nbuf){
	static char stream_size[HEAD_SIZE];		/* size of the message to be received, static to avoid allocate memory at each call*/
	ssize_t r;
	static size_t length=0 ; 				/* static because some length has to be read again */
	if (!length)  {
		bzero(stream_size, HEAD_SIZE);
		r = read(sockfd, stream_size, HEAD_SIZE);
		if (r < 0)
			dispError (fct, "Cannot read message's length (server has failed?)");
		r = sscanf (stream_size, "%lu", &length);
		if (r != 1)
			dispError (fct, "Cannot read message's length (server has failed?)");
		dispDebug (fct, 3, "prepare to receive a message of length :%lu",length);
	}
	size_t mini = length > nbuf ? nbuf: length;
	int read_length = 0;
	bzero(buf, nbuf);
	do
	{
		r = read(sockfd, buf + read_length, mini-read_length);
		if (r < 0)
			dispError(fct, "Cannot read message (called by : %s)");
		read_length += r;
	} while (read_length < mini);
  
	length -= mini; // length to be read again
	return length;
}


/* Send a string through the open socket and get acknowledgment (OK)
 * Manage connection problems
 *
 * Parameters:
 * - fct: name of the function that calls sendString (used for the logging)
 * - str: string to send
 * - ...:  accept extra parameters for str (string expansion)
 */
void sendString(const char* fct, const char* str, ...) {
	va_list args;
	va_start(args, str);
	bzero(buffer, MAX_LENGTH);
	vsprintf(buffer, str, args);
	/* check if the socket is open */
	if (sockfd < 0)
		dispError( fct, "The connection to the server is not established. Call 'connectToServer' before !");

	/* send our message */
	ssize_t r = write(sockfd, buffer, strlen(buffer));
	dispDebug(fct,2, "Send '%s' to the server", buffer);
	if (r < 0)
		dispError(fct, "Cannot write to the socket (%s)", buffer);

	/* get acknowledgment */
	size_t rr = read_inbuf(fct, buffer, MAX_LENGTH);
	if (rr > 0)
	  dispError(fct, "Acknowledgement message too long (sending:%s,receive:%s)", str,buffer);

	if (strcmp(buffer, "OK") != 0)
		dispError(fct, "Error: The server does not acknowledge, but answered:\n%s",buffer);

	dispDebug(fct, 3, "Receive acknowledgment from the server");
}



/* -------------------------------------
 * Initialize connection with the server
 * Quit the program if the connection to the server cannot be established
 *
 * Parameters:
 * - fct: name of the function that calls connectToCGS (used for the logging)
 * - serverName: (string) address of the server (it could be "localhost" if the server is run in local, or "pc4521.polytech.upmc.fr" if the server runs there)
 * - port: (int) port number used for the connection
 * - name: (string) name of the bot : max 20 characters (checked by the server)
 */
void connectToCGS(const char* fct, const char* serverName, unsigned int port, char* name)
{
	struct sockaddr_in serv_addr;
	struct hostent *server;

	/* copy the name */
	strncpy(playerName, name, 20);

	dispDebug( fct,2, "Initiate connection with %s (port: %d)", serverName, port);

	/* Create a socket point, TCP/IP protocol, connected */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		dispError( fct, "Impossible to open socket");

	/* Get the server */
	server = gethostbyname(serverName);
	if (server == NULL)
		dispError( fct, "Unable to find the server by its name");
	dispDebug( fct,1, "Open connection with the server %s", serverName);

	/* Allocate sockaddr */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy(server->h_addr, &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

	/* Now connect to the server */
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
		dispError( fct, "Connection to the server '%s' on port %d impossible.", serverName, port);

	/* Sending our name */
	sendString( fct, "CLIENT_NAME %s",name);


}


/* ----------------------------------
 * Close the connection to the server
 * to do, because we are polite
 *
 * Parameters:
 * - fct: name of the function that calls closeCGSConnection (used for the logging)
*/
void closeCGSConnection(const char* fct)
{
	if (sockfd<0)
		dispError( fct,"The connection to the server is not established. Call 'connectToServer' before !");
	close(sockfd);
}



/* ------------------------------------------------------------------------------
 * Wait for a Game, and retrieve its name and first data (typically, array sizes)
 *
 * Parameters:
 * - fct: name of the function that calls waitForGame (used for the logging)
 * - gameType: string (max 200 characters) type of the game we want to play (empty string for regular game)
 * - gameName: string (max 50 characters), game name filled by the function
 * - data: string (max 128 characters), corresponds to the data returns by the server
 *
 * gameType is a string like "GAME key1=value1 key2=value1 ..."
 * - GAME can be empty (wait for a game). It gives the type of the game
 *   it could be "TRAINING xxxx" to play against bot xxxx
 *   or "TOURNAMENT xxxx" to join the tournament xxxx
 * - key=value pairs are used for options (each training player has its own options)
 *   invalid keys are ignored, invalid values leads to error
 *   the following options are common to every training player:
 *     - 'timeout': allows an define the timeout when training (in seconds)
 *     - 'seed': allows to set the seed of the random generator
 *     - 'start': allows to set who starts ('0' or '1')
 */
void waitForGame(const char* fct, const char* gameType, char* gameName, char* data)
{
	size_t r;
	if (gameType)
	    sendString( fct,"WAIT_GAME %s", gameType);
	else
	    sendString( fct,"WAIT_GAME ");

	/* read Labyrinth name
	 If the name send is "NOT_READY", then we need to wait again
	 This (stupid) polling is here to allow the server to dectect (at least at the polling sampling period)
	 if we have disconnected or not
	 (that's the only way for the server to detect disconnection, ie sending something and check if the socket is still open)*/
	do{
        bzero(buffer,MAX_LENGTH);
        r = read_inbuf(fct,buffer,MAX_LENGTH);
        if (r>0)
            dispError( fct, "Too long answer from 'WAIT_GAME' command (sending:%s)");
    } while (strcmp(buffer,"NOT_READY")==0);

	dispDebug(fct,1, "Receive Game name=%s", buffer);
	strcpy( gameName, buffer);

	/* read Labyrinth size */
	bzero(buffer,MAX_LENGTH);
	r = read_inbuf(fct,buffer,MAX_LENGTH);
	if (r>0)
	  dispError( fct, "Answer from 'WAIT_GAME' too long");

	dispDebug( fct,2, "Receive Game sizes=%s", buffer);
	strcpy( data, buffer);
}



/* -------------------------------------
 * Get the game data and tell who starts
 * It fills the char* data with the data of the game (it will be parsed by the caller)
 * 1 if there's a wall, 0 for nothing
 *
 * Parameters:
 * - fct: name of the function that calls gameGetData (used for the logging)
 * - data: the array of game (the pointer data MUST HAVE allocated with the right size !!)
 *
 * Returns 0 if the client begins, or 1 if the opponent begins
 */
int getGameData(const char* fct, char* data, size_t ndata)
{
	sendString(fct, "GET_GAME_DATA");

	/* read game data */
	size_t r = read_inbuf(fct, data, ndata);
	if (r > 0)
		dispError( fct, "too long answer from 'GET_GAME_DATA' command");

	dispDebug( fct,2, "Receive game's data:%s", data);


	/* read if we begin (0) or if the opponent begins (1) */
	bzero(buffer,MAX_LENGTH);
	r = read_inbuf(fct,buffer,MAX_LENGTH);
	if (r > 0)
		dispError(fct, "too long answer from 'GET_GAME_DATA' ");

	dispDebug(fct,2, "Receive these player who begins=%s", buffer);

	return buffer[0] - '0';
}



/* ----------------------
 * Get the opponent move
 *
 * Parameters:
 * - fct: name of the function that calls getCGSMove (used for the logging)
 * - move: a string representing a move (the caller will parse it to extract the move's values)
 * - msg: a string with extra data (or message when the move is not a NORMAL_MOVE), max 256 char.
 *
 * move and msg are already allocated, with at least MAX_MOVE and MAX_MESSAGE chars
 * Fill the move  and string, and returns a return_code (0 for normal move, 1 for a winning move, -1 for a losing (or illegal) move)
 * this code is relative to the opponent (+1 if HE wins, ...)
 */
t_return_code getCGSMove( const char* fct, char* move ,char* msg)
{
	t_return_code result;
	sendString( fct, "GET_MOVE");
	*move = *msg = 0;

	/* read move */
	size_t r = read_inbuf(fct, move, MAX_GET_MOVE);
	if (r>0)
		dispError( fct, "too long answer from 'GET_MOVE' command");
	dispDebug(__FUNCTION__,1, "Receive that move:%s", move);

	/* read the message */
	r = read_inbuf(fct, msg, MAX_MESSAGE);
	if (r>0)
		dispError( fct, "Too long answer from 'GET_MOVE' command");
	dispDebug(__FUNCTION__,2, "Receive that return code:%s", buffer);

	/* read the return code*/
	bzero(buffer, MAX_LENGTH);
	r = read_inbuf(fct,buffer, MAX_LENGTH);
	if (r>0)
		dispError( fct, "Too long answer from 'GET_MOVE' command");
	dispDebug(__FUNCTION__,2, "Receive that return code:%s", buffer);
	sscanf( buffer, "%d", &result);

	if (result != NORMAL_MOVE)
		printf("[%s] %s\n", __FUNCTION__, msg);

	return result;
}



/* -----------
 * Send a move
 *
 * Parameters:
 * - fct: name of the function that calls sendCGSMove (used for the logging)
 * - move: a string representing a move (the caller will parse it to extract the move's values)
 * - answer: a string representing the answer (should be allocated)
 *
 * Returns a return_code (0 for normal move, 1 for a winning move, -1 for a losing (or illegal) move
 */
t_return_code sendCGSMove( const char* fct, char* move, char* answer)
{
	t_return_code result;
	sendString( fct, "PLAY_MOVE %s", move);

	/* read the associated answer */
	bzero(buffer, MAX_LENGTH);
	size_t r = read_inbuf(fct, buffer, MAX_LENGTH);
	if (r>0)
		dispError( fct, "Too long answer from 'PLAY_MOVE' command ");
	dispDebug( fct,1, "Receive that message: %s", buffer);
	if (answer)
		strcpy(answer, buffer);

	/* read return code */
	bzero(buffer, MAX_LENGTH);
	r = read_inbuf(fct, buffer, MAX_LENGTH);
	if (r>0)
		dispError(fct, "Too long answer from 'PLAY_MOVE' command");
	dispDebug(fct,2, "Receive that return code: %s", buffer);
	sscanf(buffer, "%d", &result);

	/* display the message if the move is not a NORMAL_MOVE */
	if (result != NORMAL_MOVE)
		printf("[%s] %s\n", __FUNCTION__, answer);

	return result;
}



/* ----------------------
 * Display the game
 * in a pretty way (ask the server what to print)
 *
 * Parameters:
 * - fct: name of the function that calls sendCGSMove (used for the logging)
 */
void printCGSGame(const char* fct)
{
  dispDebug( fct,2, "Try to get string to display Game");

	/* send command */
	sendString( fct, "DISP_GAME");

	/* get string to print */
	size_t r ;
	do {
	  r = read_inbuf(fct,buffer,MAX_LENGTH);
	  printf("%s",buffer);
	} while(r>0);
}



/* ----------------------------
 * Send a comment to the server
 *
 * Parameters:
 * - fct: name of the function that calls sendCGSMove (used for the logging)
 * - comment: (string) comment to send to the server (max 100 char.)
 */
void sendCGSComment(const char* fct, const char* comment)
{
  dispDebug( fct,2, "Try to send a comment");

	/* max 100. car */
	if (strlen(comment)>100)
		dispError( fct, "The Comment is more than 100 characters.");

	/* send command */
	sendString( fct, "SEND_COMMENT %s", comment);
}









//laby.h

typedef enum
{
	INSERT_LINE_LEFT = 	0,
	INSERT_LINE_RIGHT = 1,
	INSERT_COLUMN_TOP = 2,
	INSERT_COLUMN_BOTTOM = 3
} t_insertion;


/*
A move is a composed of:
- a type of insertion (that can be INSERT_LINE_LEFT, INSERT_LINE_RIGHT, INSERT_COLUMN_UP, or INSERT_COLUMN_DOWN)
- the line or column number for the insertion
- the rotation of the tile to be inserted (from 0 to 3 clockwise quarters)
- a tuple (x,y) that indicates where to move
- info on the new tile (if it has a wall on North, East, South, West, and it's item number)
- next item for the player who has played
*/
typedef struct
{
	t_insertion insert; 	/* type of the insertion */
	int number;				/* column or line number */
	int rotation;
	int x, y; 				/* coordinate where to move */
	int tileN, tileE, tileS, tileW, tileItem;	/* new tile (set by playMove or getMove) */
	int nextItem;			/* next item for the player (set by playMove or getMove) */
} t_move;




//laby.c


unsigned char nX, nY; 	/* store lab size, used for getLabyrinth (the user do not have to pass them once again */


/* -------------------------------------
 * Initialize connection with the server
 * Quit the program if the connection to the server
 * cannot be established
 *
 * Parameters:
 * - serverName: (string) address of the server
 *   (it could be "localhost" if the server is run in local,
 *   or "pc4521.polytech.upmc.fr" if the server runs there)
 * - port: (int) port number used for the connection
 * - name: (string) name of this bot (max 20 characters)
 */
void connectToServer(const char* serverName, int port, char* name)
{
	connectToCGS(__FUNCTION__, serverName, port, name);
}


/* ----------------------------------
 * Close the connection to the server
 * to do, because we are polite
 *
 * Parameters:
 * None
*/
void closeConnection()
{
	closeCGSConnection(__FUNCTION__ );
}



/* ------------------------------------------------------------------------------
 * Wait for a Game, and retrieve its name and first data (array of the labyrinth
 *
 * Parameters:
 * - gameType: string (max 200 characters) type of the game we want to play (empty string for regular game)
 * - labyrinthName: string (max 50 characters), corresponds to the game name (filled by the function)
 * - sizeX, sizeY: sizes of the labyrinth
 *
 * gameType is a string like "GAME key1=value1 key2=value1 ..."
 * - It indicates the type of the game you want to plys
 *   it could be "TRAINING <BOT>" to play against bot <BOT>
 *   or "TOURNAMENT xxxx" to join the tournament xxxx
 *   or "" (empty string) to wait for an opponent (decided by the server)
 * - key=value pairs are used for options (each training player has its own options)
 *   invalid keys are ignored, invalid values leads to error
 *   the following options are common to every training player:
 *   - timeout: allows an define the timeout when training (in seconds)
 *   - 'seed': allows to set the seed of the random generator
 *   - 'start': allows to set who starts ('0' or '1')
 *
 * The bot name <BOT> could be:
 * - "PLAY_RANDOM" for a player that make random (but legal) moves
 * - "ASTAR" for a
 *
 *
 */
void waitForLabyrinth(const char* gameType, char* labyrinthName, int* sizeX, int* sizeY)
{
	char data[128];
	/* wait for a game */
	waitForGame( __FUNCTION__, gameType, labyrinthName, data);

	/* parse the data */
	sscanf( data, "%d %d", sizeX, sizeY);

	/* store the sizes, so that we can reuse them during getLabyrinth */
	nX = *sizeX;
	nY = *sizeY;
}



/* -------------------------------------
 * Get the labyrinth and tell who starts
 * It fills the char* lab with the data of the labyrinth
 * 1 if there's a wall, 0 for nothing
 *
 * Parameters:
 * - lab: the array of labyrinth (the pointer data MUST HAVE allocated with the right size !!)
 * - tile North, East, South, West and Item : extern tile (to be later inserted)
 *
 * Returns 0 if you begin, or 1 if the opponent begins
 */
int getLabyrinth(int* lab, int* tileN, int* tileE, int* tileS, int* tileW, int* tileItem)
{
	char data[4096];
	/* wait for a game */
	int ret = getGameData( __FUNCTION__, data, 4096);

	/* copy the data in the array lab
	 * the datas is a readable string of char '0' and '1'
	 * */
	char *p = data;
	int nbchar;
	for( int i=0; i<nX*nY; i++) {
		sscanf(p, "%d %d %d %d %d %n", lab, lab+1, lab+2, lab+3, lab+4, &nbchar);
		p += nbchar;
		lab += 5;
	}

	sscanf(p, "%d %d %d %d %d", tileN, tileE, tileS, tileW, tileItem);
    return ret;
}



/* ----------------------
 * Get the opponent move
 *
 * Parameters:
 * - move: a t_move variable, filled by the function
 * Returns:
 * - NORMAL_MOVE for normal move,
 * - WINNING_MOVE for a winning move, -1
 * - LOOSING_MOVE for a losing (or illegal) move
 * - this code is relative to the opponent (WINNING_MOVE if HE wins, ...)
 */
t_return_code getMove(t_move *move)
{
    char data[MAX_GET_MOVE];
    char msg[MAX_MESSAGE];

    /* get the move */
    int ret = getCGSMove(__FUNCTION__, data, msg);

	/* extract move */
	sscanf( data, "%d %d %d %d %d %d %d %d %d %d %d", &(move->insert), &(move->number), &(move->rotation),
			&(move->x), &(move->y),
			&(move->tileN), &(move->tileE), &(move->tileS), &(move->tileW), &(move->tileItem),
			&(move->nextItem));
	dispDebug(__FUNCTION__,2,"move type:%d, ret:%d",move->insert, ret);
	return ret;
}



/* -----------
 * Send a move
 *
 * Parameters:
 * - move: a move
 *
 * Returns a return_code (0 for normal move, 1 for a winning move, -1 for a losing (or illegal) move
 */
t_return_code sendMove(t_move* move)
{
    /* build the string move */
    char data[128];
    char answer[MAX_MESSAGE];
    sprintf( data, "%d %d %d %d %d", move->insert, move->number, move->rotation, move->x, move->y);
// dispDebug(__FUNCTION__,"move send : %s",data);
    /* send the move */
	int ret = sendCGSMove( __FUNCTION__, data, answer);
	/* get the new tile */
	sscanf(answer, "%d %d %d %d %d %d", &(move->tileN), &(move->tileE), &(move->tileS), &(move->tileW), &(move->tileItem), &(move->nextItem));
	return ret;
}



/* ----------------------
 * Display the labyrinth
 * in a pretty way (ask the server what to print)
 */
void printLabyrinth()
{
    printCGSGame(__FUNCTION__ );
}



/* -----------------------------
 * Send a comment to the server
 *
 * Parameters:
 * - comment: (string) comment to send to the server (max 100 char.)
 */
void sendComment(const char* comment)
{
    sendCGSComment( __FUNCTION__, comment);
}




//main.c







typedef struct
{
	int x;
	int y;
	int tileN;
	int tileE;
	int tileS;
	int tileW;
	int tileItem;
} tile;


/*
//Chain array			NOT USED FOR NOW?
typedef struct
{
	int value;
	ArrayNode nextNode;
} ArrayNode;
*/

//function recursively marks the step on the available unmarked tiles adjacent to the tile entered in parameters (tileX ; tileY)
//laby is the labyrinth
//tileX and tileY are the coordinates of the current tile to test
//gx and gy are the coordinates of the goal
//sizeX and sizeY are the dimensions of the labyrinth
//steps is a pointer to an array of the steps for all tiles 
//currentStep is the step of the current tile (could have also been found from the steps array)
void recursivelyMarkSteps(tile *laby, int tileX, int tileY, int gx, int gy, int sizeX, int sizeY, int *steps, int currentStep) 
{
	printf("launched recursivelyMarkSteps, tile %d;%d\n",tileX,tileY);
	if (!(tileX == gx && tileY == gy))	//test if tile isn't goal
	{
		//recursively try north path :
		if (0 <= (tileY - 1) && laby[tileY*sizeX+tileX].tileN == 0)	//test if north tile exists and there isn't a north wall on our tile
		{
			if (laby[(tileY-1)*sizeX+tileX].tileS == 0 && steps[(tileY-1)*sizeX+tileX] == -1)	//test if north tile doesn't have a south wall and the step count is still -1 on that tile
			{
				printf("tile %d;%d : tileN is valid\n",tileX,tileY);
				steps[(tileY-1)*sizeX+tileX] = currentStep + 1;		//increment step of next tile (north tile)
				recursivelyMarkSteps(laby, tileX, tileY-1, gx, gy, sizeX, sizeY, steps, currentStep + 1);
			}
		}
		//recursively try east path :
		if (sizeX > (tileX + 1) && laby[tileY*sizeX+tileX].tileE == 0)	//test if east tile exists and there isn't a east wall on our tile
		{
			if (laby[tileY*sizeX+tileX+1].tileW == 0 && steps[tileY*sizeX+tileX+1] == -1)	//test if east tile doesn't have a south wall and the step count is still -1 on that tile
			{
				printf("tile %d;%d : tileE is valid\n",tileX,tileY);
				steps[tileY*sizeX+tileX+1] = currentStep + 1;		//increment step of next tile (east tile)
				recursivelyMarkSteps(laby, tileX+1, tileY, gx, gy, sizeX, sizeY, steps, currentStep + 1);
			}
		}
		//recursively try south path :
		if (sizeY > (tileY + 1) && laby[tileY*sizeX+tileX].tileS == 0)	//test if south tile exists and there isn't a south wall on our tile
		{
			if (laby[(tileY+1)*sizeX+tileX].tileN == 0 && steps[(tileY+1)*sizeX+tileX] == -1)	//test if south tile doesn't have a south wall and the step count is still -1 on that tile
			{
				printf("tile %d;%d : tileS is valid\n",tileX,tileY);
				steps[(tileY+1)*sizeX+tileX] = currentStep + 1;		//increment step of next tile (south tile)
				recursivelyMarkSteps(laby, tileX, tileY+1, gx, gy, sizeX, sizeY, steps, currentStep + 1);
			}
		}
		//recursively try west path :
		if (0 <= (tileX - 1) && laby[tileY*sizeX+tileX].tileW == 0)	//test if west tile exists and there isn't a west wall on our tile
		{
			if (laby[tileY*sizeX+tileX-1].tileE == 0 && steps[tileY*sizeX+tileX-1] == -1)	//test if west tile doesn't have a south wall and the step count is still -1 on that tile
			{
				printf("tile %d;%d : tileW is valid\n",tileX,tileY);
				steps[tileY*sizeX+tileX-1] = currentStep + 1;		//increment step of next tile (west tile)
				recursivelyMarkSteps(laby, tileX-1, tileY, gx, gy, sizeX, sizeY, steps, currentStep + 1);
			}
		}
	}
}



//function recursively adds the direction values to the path array based on the steps array to go from the player (step = 0) to the current tile (tileX ; tileY) (normally the goal tile on the first iteration)
//tileX and tileY are the coordinates of the current tile to test
//sizeX and sizeY are the dimensions of the labyrinth
//steps is a pointer to an array of the steps for all tiles 
//currentStep is the step of the current tile (could have also been found from the steps array)
//maxSteps is the highest step (the step of the goal)
//path is an array with the directions to go from the player to the goal (N = 0, E = 1, S = 2, W = 3)
void recursivelyAddPathValuesFromSteps(int tileX, int tileY, int sizeX, int sizeY, int *steps, int currentStep, int maxStep, int *path)
{
	printf("launched recursivelyAddPathValuesFromSteps\n");
	if (currentStep != 0)
	{
		//recursively try north path :
		if (0 <= tileY - 1)	//test if north tile exists
		{
			if (steps[(tileY-1)*sizeX+tileX] == currentStep - 1)	//test if the north tile is the previous tile in the path
			{
				path[maxStep - currentStep + 1] = 2;		//add south direction value to path
				return recursivelyAddPathValuesFromSteps(tileX, tileY - 1, sizeX, sizeY, steps, currentStep - 1, maxStep, path);
			}
		}
		//recursively try east path :
		if (sizeX > tileX + 1)	//test if east tile exists
		{
			if (steps[tileY*sizeX+tileX+1] == currentStep - 1)	//test if the east tile is the previous tile in the path
			{
				path[maxStep - currentStep + 1] = 3;		//add west direction value to path
				return recursivelyAddPathValuesFromSteps(tileX + 1, tileY, sizeX, sizeY, steps, currentStep - 1, maxStep, path);
			}
		}
		//recursively try south path :
		if (sizeY > tileY + 1)	//test if south tile exists
		{
			if (steps[(tileY+1)*sizeX+tileX] == currentStep - 1)	//test if the south tile is the previous tile in the path
			{
				path[maxStep - currentStep + 1] = 0;		//add north direction value to path
				return recursivelyAddPathValuesFromSteps(tileX, tileY + 1, sizeX, sizeY, steps, currentStep - 1, maxStep, path);
			}
		}
		//recursively try west path :
		if (0 <= tileX - 1)	//test if west tile exists
		{
			if (steps[tileY*sizeX+tileX-1] == currentStep - 1)	//test if the west tile is the previous tile in the path
			{
				path[maxStep - currentStep + 1] = 1;		//add east direction value to path
				return recursivelyAddPathValuesFromSteps(tileX - 1, tileY, sizeX, sizeY, steps, currentStep - 1, maxStep, path);
			}
		}
	}
}

/*
Function to test if a path is available From Player to current goal
If not : returns null adress?
If yes : allocates an array with the moves of the first path found, array starts with the array's length : returns the adress of the array
in returned path array : 0 = North, East = 1, South = 2, West = 3
Arg:
*laby = labyrinth adress
px and py = player x and player y coordinates
gx and gy = goal x and goal y coordinates
sizeX and sizeY are the dimensions of the labyrinth
*/
int* testPathPlayerToGoal(tile * laby, int px, int py, int gx, int gy, int sizeX, int sizeY)
{
	printf("launched testPathPlayerGoal\n");
	//creation of an array with the number of steps distance from player, initialize all values at 0
	int steps[sizeY][sizeX];
	for (int i = 0; i < sizeY; i++)
	{
		for (int j = 0; j < sizeX; j++)
		{
			steps[i][j] = -1;
		}		
	}

	steps[py][px] = 0;
	printf("launching recursivelyMarkSteps\n");
	//mark recursively steps :
	recursivelyMarkSteps(laby, px, py, gx, gy, sizeX, sizeY, steps, 0);
	printf("recursivelyMarkSteps exited\n");

	// if no path has been found :
	if (steps[gy][gx] == -1)
	{
		return NULL;
	}
	
	//allocate path array :
	int* path = malloc( (sizeof(int)) * (steps[gy][gx] + 1) );	//allocate an array of size = number of steps to goal + 1
	path[0] = steps[gy][gx] + 1;	//first element value is the size of the array

	printf("launching recursivelyAddPathValuesFromSteps\n");
	//get recursively path values from steps array to path array :
	recursivelyAddPathValuesFromSteps(gx, gy, sizeX, sizeY, steps, steps[gy][gx], steps[gy][gx], path);
	printf("recursivelyAddPathValuesFromSteps exited\n");
	return path;
}

int PlayMove(tile *laby, int px, int py, int gx, int gy, int sizeX, int sizeY)
{
	printf("PlayMove launched\n");
	int result;
	printf("launching testPathPlayerGoal\n");
	int* path = testPathPlayerToGoal(laby, px, py, gx, gy, sizeX, sizeY);
	printf("testPathPlayerGoal exited\n");
//	if (path != NULL)		//not final yet, just for testing
//	{
//		move = {.insert = 0, .number = 7, .rotation = 3, .x = 0, .y = 0, .tileN = 1, .tileE = 0, .tileS = 0, .tileW = 0, .nextItem = 2};
//	}
	free(path);
	t_move move = {.insert = 0, .number = 5, .rotation = 3, .x = gx, .y = gy, .tileN = 1, .tileE = 0, .tileS = 0, .tileW = 0, .nextItem = 2};
	
	result = sendMove(&move);
	printf("%d\n",result);
	return result;
}


int main(void)
{
	//connection to server
	printf("connection to server :");
	connectToServer("172.105.76.204", 5678, "VFtest");
	printf(" success\n");

	int sizeX;		//labyrinth size
	int sizeY;		//labyrinth size
	int playerX = 0, playerY = 0;
	char name[50] = "test";

	printf("waiting for labyrinth :");
	waitForLabyrinth("TRAINING DONTMOVE timeout=10 seed=177013 start=0", name, &sizeX, &sizeY);
	printf(" success\n");
	printf("name is %s\n", name);
	int LabyrinthReceivedTab[5 * sizeX * sizeY];

	//external tile declaration :
	tile extern_tile = {.x = -1, .y = -1, .tileN = 0, .tileE = 0, .tileS = 0, .tileW = 0, .tileItem = 0};

	//getting labyrinth :	returns turn (0 = player, 1 = opponent)
	printf("getting labyrinth :");
	int turn = getLabyrinth(LabyrinthReceivedTab, &extern_tile.tileN, &extern_tile.tileE, &extern_tile.tileS, &extern_tile.tileW, &extern_tile.tileItem);
	printf(" success\n");

	//initializing labyrinth
	tile Laby[sizeY][sizeX];
	int lenLaby = sizeX*sizeY;

	for (int i = 0; i < sizeY; i++)
	{
		for (int j = 0; j < sizeX; j++)
		{
			tile tileToAdd = {.x = j, .y = i, .tileN = LabyrinthReceivedTab[(i*sizeX+j)*5+0], .tileE = LabyrinthReceivedTab[(i*sizeX+j)*5+1], .tileS = LabyrinthReceivedTab[(i*sizeX+j)*5+2], .tileW = LabyrinthReceivedTab[(i*sizeX+j)*5+3], .tileItem = LabyrinthReceivedTab[(i*sizeX+j)*5+4]};
			Laby[i][j] = tileToAdd;
		}
	}
// testing:
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[0][0].x,Laby[0][0].y,Laby[0][0].tileN,Laby[0][0].tileE,Laby[0][0].tileS,Laby[0][0].tileW,Laby[0][0].tileItem);
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[0][1].x,Laby[0][1].y,Laby[0][1].tileN,Laby[0][1].tileE,Laby[0][1].tileS,Laby[0][1].tileW,Laby[0][1].tileItem);
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[1][0].x,Laby[1][0].y,Laby[1][0].tileN,Laby[1][0].tileE,Laby[1][0].tileS,Laby[1][0].tileW,Laby[1][0].tileItem);
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[1][1].x,Laby[1][1].y,Laby[1][1].tileN,Laby[1][1].tileE,Laby[1][1].tileS,Laby[1][1].tileW,Laby[1][1].tileItem);
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[0][sizeX-1].x,Laby[0][sizeX-1].y,Laby[0][sizeX-1].tileN,Laby[0][sizeX-1].tileE,Laby[0][sizeX-1].tileS,Laby[0][sizeX-1].tileW,Laby[0][sizeX-1].tileItem);
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[sizeY-1][0].x,Laby[sizeY-1][0].y,Laby[sizeY-1][0].tileN,Laby[sizeY-1][0].tileE,Laby[sizeY-1][0].tileS,Laby[sizeY-1][0].tileW,Laby[sizeY-1][0].tileItem);
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[sizeY-1][sizeX-1].x,Laby[sizeY-1][sizeX-1].y,Laby[sizeY-1][sizeX-1].tileN,Laby[sizeY-1][sizeX-1].tileE,Laby[sizeY-1][sizeX-1].tileS,Laby[sizeY-1][sizeX-1].tileW,Laby[sizeY-1][sizeX-1].tileItem);


	//start game
	while (1)
	{
		printLabyrinth();
		if (turn == 0)			//player turn
		{
			int xdest = 7;
			int ydest = 7;
//			scanf("%d%d",&xdest,&ydest);
			printf("our turn, launching PlayMove\n");
			int playMoveResult = PlayMove(Laby, playerX, playerY, xdest, ydest, sizeX, sizeY);
			printf("playmove result = %d\n",playMoveResult);
			if(playMoveResult == -1)
			{
				goto fin;
			}

			turn = 1;
		}
		else					//opponent turn
		{
			t_move ennemy_move;
			if(getMove(&ennemy_move)==0){
				printf("normal opponent move");
			}
			turn = 0;
		}
	}
	fin:
	//disconnecting from server
	printf("disconnecting from server\n");
	closeConnection();


	printf("\n%d\n",extern_tile.tileN);
	printf("\n%d\n",extern_tile.tileE);
	printf("\n%d\n",extern_tile.tileS);
	printf("\n%d\n",extern_tile.tileW);
	printf("\n%d\n",extern_tile.tileItem);	

	return 1;
}




/*
Strategy
if win in 1 :
	test for most disadventagous move for opponent

else :
	search path to closest to goal while disadventagous move for opponent


*/