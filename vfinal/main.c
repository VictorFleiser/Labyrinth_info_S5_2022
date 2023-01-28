#include <stdio.h>
#include <stdlib.h>
#include "labyrinthAPI.h"

//tile structure
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

//player structure
typedef struct
{
	int x;
	int y;
	int item;
} t_player;

//returns the equivalent index in a 1D array from the x and y coordinates of a 2D array
int f2Dto1D(int xcoord, int ycoord, int sizeX)
{
	return (ycoord*sizeX+xcoord);
}

//returns the x coordinate in a 2D array from the index of a 1D array
int f1Dto2Dx(int coord, int sizeX)
{
	return (coord%sizeX);
}

//returns the y coordinate in a 2D array from the index of a 1D array
int f1Dto2Dy(int coord, int sizeX)
{
	return (coord/sizeX);
}

//Function rotates the tileToRotate by numberOfRotations clockwise
void rotateTile(tile * tileToRotate, int numberOfRotations)
{
	for (int i = 0; i < numberOfRotations; i++)
	{
		int tp = tileToRotate->tileN;
		tileToRotate->tileN = tileToRotate->tileW;
		tileToRotate->tileW = tileToRotate->tileS;
		tileToRotate->tileS = tileToRotate->tileE;
		tileToRotate->tileE = tp;
	}
}

//copy tile contents of src to dest
void copyTileContent(tile * dest, tile src)
{
	dest->tileItem = src.tileItem;
	dest->tileN = src.tileN;
	dest->tileE = src.tileE;
	dest->tileS = src.tileS;
	dest->tileW = src.tileW;
}

//Function updates the labyrinth (laby) with the move played in parameters (move), also updates the extern tile and player
void updateGame(tile *laby, t_move move, int sizeX, int sizeY, tile *extern_tile, t_player *activePlayer, t_player *passivPlayer)
{
	//rotating the extern tile
	rotateTile(extern_tile, move.rotation);

	//moving the tiles
	switch (move.insert)
	{
	case 0 :												//insert line left
		if (passivPlayer->y == move.number)					//test if the passiv player is on the line
		{
			passivPlayer->x = ((passivPlayer->x + 1)%sizeX + sizeX) % sizeX;								//move passive player 1 right
		}
		for (int i = f2Dto1D(sizeX-1, move.number, sizeX); i > f2Dto1D(0, move.number, sizeX); i--)		//go through all tiles of selected line (except first tile) in reverse order
		{
			//copy left tile
			copyTileContent(&laby[i], laby[i-1]);
		}
		//adding the previous extern tile to board
		copyTileContent(&laby[f2Dto1D(0, move.number, sizeX)], *extern_tile);
		break;

	case 1 :												//insert line right
		if (passivPlayer->y == move.number)					//test if the passiv player is on the line
		{
			passivPlayer->x = ((passivPlayer->x - 1)%sizeX + sizeX) % sizeX;								//move passive player 1 left
		}
		for (int i = f2Dto1D(0, move.number, sizeX); i < f2Dto1D(sizeX - 1, move.number, sizeX); i++)	//go through all tiles of selected line (except last tile) in order
		{
			//copy right tile
			copyTileContent(&laby[i], laby[i+1]);
		}
		//adding the previous extern tile to board
		copyTileContent(&laby[f2Dto1D(sizeX - 1, move.number, sizeX)], *extern_tile);
		break;	
	case 2 :												//insert column top
		if (passivPlayer->x == move.number)					//test if the passiv player is on the column
		{
			passivPlayer->y = ((passivPlayer->y + 1)%sizeY + sizeY) % sizeY;								//move passive player 1 down
		}
		for (int i = f2Dto1D(move.number, sizeY - 1, sizeX); i > f2Dto1D(move.number, 0, sizeX); i = i - sizeX)		//go through all tiles of selected column (except first tile) in reverse order
		{
			//copy upward tile
			copyTileContent(&laby[i], laby[i - sizeX]);
		}
		//adding the previous extern tile to board
		copyTileContent(&laby[f2Dto1D(move.number, 0, sizeX)], *extern_tile);
		break;
	case 3 :												//insert column bottom
		if (passivPlayer->x == move.number)					//test if the passiv player is on the column
		{
			passivPlayer->y = ((passivPlayer->y - 1)%sizeY + sizeY) % sizeY;								//move passive player 1 up
		}
		for (int i = f2Dto1D(move.number, 0, sizeX); i < f2Dto1D(move.number, sizeY - 1, sizeX); i = i + sizeX)		//go through all tiles of selected column (except last tile) in order
		{
			//copy downward tile
			copyTileContent(&laby[i], laby[i + sizeX]);
		}
		//adding the previous extern tile to board
		copyTileContent(&laby[f2Dto1D(move.number, sizeY - 1, sizeX)], *extern_tile);
		break;
	default:												//Should never happen
		printf("move insert value impossible !!!!!\n");
		break;
	}

	//saving the new extern tile
	extern_tile->tileItem = move.tileItem;
	extern_tile->tileN = move.tileN;
	extern_tile->tileE = move.tileE;
	extern_tile->tileS = move.tileS;
	extern_tile->tileW = move.tileW;
	
	//updating active player
	activePlayer->x = move.x;
	activePlayer->y = move.y;
	activePlayer->item = move.nextItem;
}

//Function updates the labyrinth (laby) with the move represented by the parameters
void updateLaby(tile *laby, int insert, int number, int rotation, int sizeX, int sizeY, tile * extern_tile, t_player *activePlayer, t_player *passivePlayer)
{
	//rotating the extern tile
	rotateTile(extern_tile, rotation);

	//moving the tiles
	switch (insert)
	{
	case 0 :													//insert line left
		if (activePlayer->y == number)							//test if the active player is on the line
		{
			activePlayer->x = ((activePlayer->x + 1) % sizeX + sizeX) % sizeX;		//move active player 1 right
		}
		if (passivePlayer->y == number)							//test if the passive player is on the line
		{
			passivePlayer->x = ((passivePlayer->x + 1)%sizeX + sizeX) % sizeX;	//move passive player 1 right
		}
		for (int i = f2Dto1D(sizeX-1, number, sizeX); i > f2Dto1D(0, number, sizeX); i--)		//go through all tiles of selected line (except first tile) in reverse order
		{
			//copy left tile
			copyTileContent(&laby[i], laby[i-1]);
		}
		//adding the previous extern tile to board
		copyTileContent(&laby[f2Dto1D(0, number, sizeX)], *extern_tile);
		break;

	case 1 :													//insert line right
		if (activePlayer->y == number)							//test if the active player is on the line
		{
			activePlayer->x = ((activePlayer->x - 1)%sizeX + sizeX) % sizeX;		//move active player 1 left
		}
		if (passivePlayer->y == number)							//test if the passive player is on the line
		{
			passivePlayer->x = ((passivePlayer->x - 1)%sizeX + sizeX) % sizeX;	//move passive player 1 left
		}
		for (int i = f2Dto1D(0, number, sizeX); i < f2Dto1D(sizeX - 1, number, sizeX); i++)	//go through all tiles of selected line (except last tile) in order
		{
			//copy right tile
			copyTileContent(&laby[i], laby[i+1]);
		}
		//adding the previous extern tile to board
		copyTileContent(&laby[f2Dto1D(sizeX - 1, number, sizeX)], *extern_tile);
		break;	
	case 2 :													//insert column top
		if (activePlayer->x == number)							//test if the active player is on the column
		{
			activePlayer->y = ((activePlayer->y + 1)%sizeY + sizeY) % sizeY;		//move active player 1 down
		}
		if (passivePlayer->x == number)							//test if the passive player is on the column
		{
			passivePlayer->y = ((passivePlayer->y + 1)%sizeY + sizeY) % sizeY;	//move passive player 1 down
		}
		for (int i = f2Dto1D(number, sizeY - 1, sizeX); i > f2Dto1D(number, 0, sizeX); i = i - sizeX)		//go through all tiles of selected column (except first tile) in reverse order
		{
			//copy upward tile
			copyTileContent(&laby[i], laby[i - sizeX]);
		}
		//adding the previous extern tile to board
		copyTileContent(&laby[f2Dto1D(number, 0, sizeX)], *extern_tile);
		break;
	case 3 :													//insert column bottom
		if (activePlayer->x == number)							//test if the active player is on the column
		{
			activePlayer->y = ((activePlayer->y - 1)%sizeY + sizeY) % sizeY;		//move active player 1 up
		}
		if (passivePlayer->x == number)							//test if the passive player is on the column
		{
			passivePlayer->y = ((passivePlayer->y - 1)%sizeY + sizeY) % sizeY;	//move passive player 1 up
		}
		for (int i = f2Dto1D(number, 0, sizeX); i < f2Dto1D(number, sizeY - 1, sizeX); i = i + sizeX)		//go through all tiles of selected column (except last tile) in order
		{
			//copy downward tile
			copyTileContent(&laby[i], laby[i + sizeX]);
		}
		//adding the previous extern tile to board
		copyTileContent(&laby[f2Dto1D(number, sizeY - 1, sizeX)], *extern_tile);
		break;
	default:												//Should never happen
		printf("move insert value impossible !!!!!\n");
		break;
	}
}

//returns the distance between 2 tiles (diagonals count as 2)
int calcDist(int px, int py, int gx, int gy)
{
	return (abs(px - gx) + abs(py - gy));
}

//function marks the distance of the tile (tileX ; tileY) to goal and recursively launches the function again on the available unmarked tiles adjacent
void recursivelyMarkDistance(tile *laby, int tileX, int tileY, int gx, int gy, int sizeX, int sizeY, int *distance) 
{
	//mark distance of current tile
	distance[f2Dto1D(tileX, tileY, sizeX)] = calcDist(tileX, tileY, gx, gy);
//	printf("case = %d;%d distance = %d\n",tileX, tileY, calcDist(tileX, tileY, gx, gy));

	//recursively try north path :
	if (0 <= (tileY - 1) && laby[f2Dto1D(tileX, tileY, sizeX)].tileN == 0)	//test if north tile exists and there isn't a north wall on our tile
	{
		if (laby[f2Dto1D(tileX, tileY - 1, sizeX)].tileS == 0 && distance[f2Dto1D(tileX, tileY - 1, sizeX)] == 999)	//test if north tile doesn't have a south wall and the distance count is still 999 on that tile
		{
			recursivelyMarkDistance(laby, tileX, tileY-1, gx, gy, sizeX, sizeY, distance);
		}
	}
	//recursively try east path :
	if (sizeX > (tileX + 1) && laby[f2Dto1D(tileX, tileY, sizeX)].tileE == 0)	//test if east tile exists and there isn't a east wall on our tile
	{
		if (laby[f2Dto1D(tileX + 1, tileY, sizeX)].tileW == 0 && distance[f2Dto1D(tileX + 1, tileY, sizeX)] == 999)	//test if east tile doesn't have a south wall and the distance count is still 999 on that tile
		{
			recursivelyMarkDistance(laby, tileX+1, tileY, gx, gy, sizeX, sizeY, distance);
		}
	}
	//recursively try south path :
	if (sizeY > (tileY + 1) && laby[f2Dto1D(tileX, tileY, sizeX)].tileS == 0)	//test if south tile exists and there isn't a south wall on our tile
	{
		if (laby[f2Dto1D(tileX, tileY + 1, sizeX)].tileN == 0 && distance[f2Dto1D(tileX, tileY + 1, sizeX)] == 999)	//test if south tile doesn't have a south wall and the distance count is still 999 on that tile
		{
			recursivelyMarkDistance(laby, tileX, tileY+1, gx, gy, sizeX, sizeY, distance);
		}
	}
	//recursively try west path :
	if (0 <= (tileX - 1) && laby[f2Dto1D(tileX, tileY, sizeX)].tileW == 0)	//test if west tile exists and there isn't a west wall on our tile
	{
		if (laby[f2Dto1D(tileX - 1, tileY, sizeX)].tileE == 0 && distance[f2Dto1D(tileX - 1, tileY, sizeX)] == 999)	//test if west tile doesn't have a south wall and the distance count is still 999 on that tile
		{
			recursivelyMarkDistance(laby, tileX-1, tileY, gx, gy, sizeX, sizeY, distance);
		}
	}
}

//Function to test if a path is available From Player coordinate to goal coordinates, returns the minimum distance to the goal from the best destination found (also saved in destX and destY)
int testPathPlayerToGoalV2(tile * laby, int px, int py, int gx, int gy, int *destX, int *destY, int sizeX, int sizeY)
{
	//creation of an array with the distance from the goal, initialize all values at 999
	int distance[sizeY*sizeX];
	for (int i = 0; i < sizeX*sizeY; i++)
	{
		distance[i] = 999;
	}

	//mark recursively distance :
	recursivelyMarkDistance(laby, px, py, gx, gy, sizeX, sizeY, distance);

	//searching best distance found
	int bestDistanceFound = 999;
	int bestXFound;
	int bestYFound;

	for (int i = 0; i < sizeY; i++)
	{
		for (int j = 0; j < sizeX; j++)
		{
			if (distance[f2Dto1D(j, i, sizeX)] < bestDistanceFound)
			{
				//new best movement tile found
				bestDistanceFound = distance[f2Dto1D(j, i, sizeX)];
				bestXFound = j;
				bestYFound = i;
			}
		}
	}
	*destX = bestXFound;
	*destY = bestYFound;
	return bestDistanceFound;
}

//Struct saving the move currently tested
typedef struct
{
	int insert;
	int number;
	int rotation;
	int activPlayerX;		//x position of best destination found
	int activPlayerY;		//y position of best destination found
	int activPlayerDist;		//distance between best destination found and goal for active Player
	int passivPlayerDist;		//distance between best destination found and goal for passive Player
} t_possibleMove;

//Function copies an array of tiles to another array of tiles
void cpyBoard(tile *dest, tile *src, int sizeX, int sizeY)
{
	for (int i = 0; i < sizeX*sizeY; i++)
	{
		dest[i] = src[i];
	}
}

//function finds the goal coordinates and saves them in xGoal and yGoal, returns 0 if the goal is in the extern tile and 1 otherwise
int findCoordsGoal(tile *laby, int sizeX, int sizeY, int goal, int *xGoal, int *yGoal, tile externTile)
{
	for (int i = 0; i < sizeX*sizeY; i++)
	{
		if (laby[i].tileItem == goal)
		{
			*xGoal = f1Dto2Dx(i, sizeX);
			*yGoal = f1Dto2Dy(i, sizeX);
			return 1;
		}
	}
	if (externTile.tileItem == goal)
	{
		return 0;
	}	
//	printf("\nERROR : goal not found!!! (returning the same as if goal on extern tile)\n");
	return 0;
}

//returns the best move found (DOES NOT RETURN THE tileN, tileE, tileS, tileW, tileItem, nextItem)
t_move findBestMoveV1(tile *laby, int sizeX, int sizeY, tile externTile, t_player activPlayer, t_player passivPlayer, t_move previous_move)
{
	//Creating a new board with the contents of laby
	tile nLaby[sizeY*sizeX];
	cpyBoard(nLaby, laby, sizeX, sizeY);
	tile cExternTile = externTile;

	t_possibleMove bestMove = {.activPlayerX = activPlayer.x, .activPlayerY = activPlayer.y , .activPlayerDist = 999};		//declared with an impossibly high best distance found to be replaced at first iteration
	t_possibleMove cMove = {.activPlayerX = activPlayer.x, .activPlayerY = activPlayer.y, .activPlayerDist = 100};	//current move being tested (default goal = player pos)
	t_move moveToTest = {.tileItem = externTile.tileItem, .tileN = externTile.tileN, .tileE = externTile.tileE, .tileS = externTile.tileS, .tileW = externTile.tileW};
	int currentGoalX = activPlayer.x;
	int currentGoalY = activPlayer.y;
	t_player currentActivPlayer = activPlayer;
	t_player currentPassivPlayer = passivPlayer;

	for (int i = 0; i < 4; i++)							//Insert
	{
		cMove.insert = i;
		moveToTest.insert = i;
		for (int r = 0; r < 4; r++)						//Rotation
		{
			cMove.rotation = r;
			moveToTest.rotation = r;
			if (i < 2)
			{
				for (int n = 1; n < sizeY; n = n + 2)	//Number
				{
					cMove.number = n;
					moveToTest.number = n;

					//test if cMove is the opposite of previous move
					if (!( (n == previous_move.number) && ( ((i == 0) && (previous_move.insert == 1)) || ((i == 1) && (previous_move.insert == 0)) || ((i == 2) && (previous_move.insert == 3)) || ((i == 3) && (previous_move.insert == 2)) ) ))
					{
						//modify nLaby with tested move
						updateLaby(nLaby, i, n, r, sizeX, sizeY, &cExternTile, &currentActivPlayer, &currentPassivPlayer);

						//find position of new goal, only enters if goal is found on the board
						if(findCoordsGoal(nLaby, sizeX, sizeY, activPlayer.item, &currentGoalX, &currentGoalY, cExternTile))
						{
//							printf("goal pos : %d, %d\n", currentGoalX, currentGoalY);
							cMove.activPlayerDist = testPathPlayerToGoalV2(nLaby, currentActivPlayer.x, currentActivPlayer.y, currentGoalX, currentGoalY, &cMove.activPlayerX, &cMove.activPlayerY, sizeX, sizeY);
						}

						//test if current move better than best move found so far :
						if (cMove.activPlayerDist < bestMove.activPlayerDist)
						{
							bestMove = cMove;
						}
					}

					//Reset the labyrinth :
					cpyBoard(nLaby, laby, sizeX, sizeY);
					currentActivPlayer = activPlayer;
					currentPassivPlayer = passivPlayer;
					cMove.activPlayerDist = 999;
					cExternTile = externTile;
				}
			}
			else
			{
				for (int n = 1; n < sizeX; n = n + 2)	//Number
				{														//CODE EXACTLY THE SAME AS ABOVE
					cMove.number = n;
					moveToTest.number = n;

					//test if cMove is the opposite of previous move
					if (!( (n == previous_move.number) && ( ((i == 0) && (previous_move.insert == 1)) || ((i == 1) && (previous_move.insert == 0)) || ((i == 2) && (previous_move.insert == 3)) || ((i == 3) && (previous_move.insert == 2)) ) ))
					{
						//modify nLaby with tested move
						updateLaby(nLaby, i, n, r, sizeX, sizeY, &cExternTile, &currentActivPlayer, &currentPassivPlayer);

						//find position of new goal, only enters if goal is found on the board
						if(findCoordsGoal(nLaby, sizeX, sizeY, activPlayer.item, &currentGoalX, &currentGoalY, cExternTile))
						{
							cMove.activPlayerDist = testPathPlayerToGoalV2(nLaby, currentActivPlayer.x, currentActivPlayer.y, currentGoalX, currentGoalY, &cMove.activPlayerX, &cMove.activPlayerY, sizeX, sizeY);
						}

						//test if current move better than best move found so far :
						if (cMove.activPlayerDist < bestMove.activPlayerDist)
						{
							bestMove = cMove;
						}
					}

					//Reset the labyrinth :
					cpyBoard(nLaby, laby, sizeX, sizeY);
					currentActivPlayer = activPlayer;
					currentPassivPlayer = passivPlayer;
					cMove.activPlayerDist = 999;
					cExternTile = externTile;
				}
			}
		}
	}
	
	//translating bestMove into an actual t_move
	t_move returnMove = {.insert = bestMove.insert, .number = bestMove.number, .rotation = bestMove.rotation, .x = bestMove.activPlayerX, .y = bestMove.activPlayerY, .tileItem = externTile.tileItem, .tileN = externTile.tileN, .tileE = externTile.tileE, .tileS = externTile.tileS, .tileW = externTile.tileW};	//extern tile is the same as the initial one, same with goal
	return returnMove;
}

//function finds the move to play, plays it and updates the game
int playMoveV2(tile *laby, int sizeX, int sizeY, tile * externTile, t_player * activPlayer, t_player * passivPlayer, t_move previous_move)
{
	//finding move to play
	t_move MoveToPlay;
	MoveToPlay = findBestMoveV1(laby, sizeX, sizeY, *externTile, *activPlayer, *passivPlayer, previous_move);

	//Updating Game
	updateGame(laby, MoveToPlay, sizeX, sizeY, externTile, activPlayer, passivPlayer);

	//Sending Move
	int result = sendMove(&MoveToPlay);

	//Gestion du réusltat :
	externTile->tileItem = MoveToPlay.tileItem;
	externTile->tileN = MoveToPlay.tileN;
	externTile->tileE = MoveToPlay.tileE;
	externTile->tileS = MoveToPlay.tileS;
	externTile->tileW = MoveToPlay.tileW;
	activPlayer->item = MoveToPlay.nextItem;

	return result;
}

//function plays one game
int playGame(void)
{

	int sizeX;		//labyrinth size
	int sizeY;		//labyrinth size

	char name[50] = "test";

	printf("waiting for labyrinth :");
	waitForLabyrinth("TRAINING RANDOM timeout=10", name, &sizeX, &sizeY);	//DONTMOVE  optional parameter :    seed=0xf11354	 start=0	TRAINING RANDOM
	printf(" success\n");
	printf("name is %s\n", name);
	int LabyrinthReceivedTab[5 * sizeX * sizeY];

	//external tile declaration :
	tile extern_tile = {.x = -1, .y = -1, .tileN = 0, .tileE = 0, .tileS = 0, .tileW = 0, .tileItem = 0};

	//getting labyrinth :	returns turn (0 = victor, 1 = opponent)
	printf("getting labyrinth :");
	int turn = getLabyrinth(LabyrinthReceivedTab, &extern_tile.tileN, &extern_tile.tileE, &extern_tile.tileS, &extern_tile.tileW, &extern_tile.tileItem);
	printf(" success\n");

	//declaring players :
	t_player victor;
	t_player opponent;
	
	t_move ennemy_move;

	if (turn == 0)	//turn victor
	{
		victor.x = 0;
		victor.y = 0;
		victor.item = 1;
		opponent.x = sizeX - 1;
		opponent.y = sizeY - 1;
		opponent.item = 24;
	}
	else			//turn opponent
	{
		opponent.x = 0;
		opponent.y = 0;
		opponent.item = 1;
		victor.x = sizeX - 1;
		victor.y = sizeY - 1;
		victor.item = 24;
	}


	//initializing labyrinth
	tile Laby[sizeY*sizeX];

	for (int i = 0; i < sizeX*sizeY; i++)
	{
		tile tileToAdd = {.x = i%sizeX, .y = i/sizeX, .tileN = LabyrinthReceivedTab[i*5+0], .tileE = LabyrinthReceivedTab[i*5+1], .tileS = LabyrinthReceivedTab[i*5+2], .tileW = LabyrinthReceivedTab[i*5+3], .tileItem = LabyrinthReceivedTab[i*5+4]};
		Laby[i] = tileToAdd;
	}

	int nbTurns = 0;

	//start game
	while (1)
	{
//		printLabyrinth();
		if (turn == 0)			//player turn
		{
			nbTurns++;
			int result = playMoveV2(Laby, sizeX, sizeY, &extern_tile, &victor, &opponent, ennemy_move);

			if(result != 0)
			{
				if (result == 1)
				{
					printf("\tWE WON : GAME WON !\n");
				}
				if (result == -1)
				{
					printf("\tWE LOST : GAME LOST !\n");
				}
				printf("number of turns : %d\n", nbTurns);
				return nbTurns;
			}

			turn = 1;
		}
		else					//opponent turn
		{
			int ennemyMoveResult = getMove(&ennemy_move);

			updateGame(Laby, ennemy_move, sizeX, sizeY, &extern_tile, &opponent, &victor);

			if(ennemyMoveResult != 0){
				if (ennemyMoveResult == 1)
				{
					printf("\tOPPONENT WON : GAME LOST !\n");
				}
				if (ennemyMoveResult == -1)
				{
					printf("\tOPPONENT LOST : GAME WON !\n");
				}

				printf("number of turns : %d\n", nbTurns);
				return nbTurns;
			}
			turn = 0;
		}
	}

	printf("number of turns : %d\n", nbTurns);
	return nbTurns;
}

int main(void)
{
	//connection to server
	printf("connection to server :");
	connectToServer("172.105.76.204", 5678, "VF1");
	printf(" success\n");

	//Number of games to be played :
	int numberOfGamesLeft = 3;

	//calcul average variables
	int NumberTurnsArray[numberOfGamesLeft]; 
	int gamePlayed = 0;

	//Plays numberOfGamesLeft games
	while (numberOfGamesLeft)
	{
		printf("\n\t\tNUMBER OF GAMES LEFT : %d\n\n",numberOfGamesLeft);
		NumberTurnsArray[gamePlayed] = playGame();
		numberOfGamesLeft--;
		gamePlayed++;
	}
	
	//calcul average
	int sumTurns = 0;
	for (int i = 0; i < gamePlayed; i++)
	{
		sumTurns += NumberTurnsArray[i];
	}
	float averageTurns = sumTurns/gamePlayed;
	printf("\n\t\taverage Turns per game : %f\n\n",averageTurns);

	//disconnecting from server
	printf("disconnecting from server\n");
	closeConnection();
}