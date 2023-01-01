#include <stdio.h>
#include <stdlib.h>
#include "labyrinthAPI.h"






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


typedef struct
{
	int x;
	int y;
	int item;
} t_player;


/*
//Chain array			NOT USED FOR NOW?
typedef struct
{
	int value;
	ArrayNode nextNode;
} ArrayNode;
*/




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

//Function updates the labyrinth (laby) with the move played in parameters (move), also updates the extern tile
void updateGame(tile *laby, t_move move, int sizeX, int sizeY, tile *extern_tile, t_player *activePlayer, t_player *passivPlayer)
{
	//rotating the extern tile
	rotateTile(extern_tile, move.rotation);

	//moving the tiles
	switch (move.insert)
	{
	case 0 :												//insert line left
		if (passivPlayer->y == move.number)					//test if the passiv player is on the left tile
		{
			passivPlayer->x = (passivPlayer->x + 1)%sizeX;								//move passive player 1 right
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
		if (passivPlayer->y == move.number)					//test if the passiv player is on the right tile
		{
			passivPlayer->x = (passivPlayer->x - 1)%sizeX;								//move passive player 1 left
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
		if (passivPlayer->x == move.number)					//test if the passiv player is on the upward tile
		{
			passivPlayer->y = (passivPlayer->y + 1)%sizeY;								//move passive player 1 down
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
		if (passivPlayer->x == move.number)					//test if the passiv player is on the downward tile
		{
			passivPlayer->y = (passivPlayer->y - 1)%sizeY;								//move passive player 1 up
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
void updateLaby(tile *laby, int insert, int number, int rotation, int sizeX, int sizeY, tile extern_tile)
{
	//rotating the extern tile
	rotateTile(&extern_tile, rotation);

	//moving the tiles
	switch (insert)
	{
	case 0 :												//insert line left
		for (int i = f2Dto1D(sizeX-1, number, sizeX); i > f2Dto1D(0, number, sizeX); i--)		//go through all tiles of selected line (except first tile) in reverse order
		{
			//copy left tile
			copyTileContent(&laby[i], laby[i-1]);
		}
		//adding the previous extern tile to board
		copyTileContent(&laby[f2Dto1D(0, number, sizeX)], extern_tile);
		break;

	case 1 :												//insert line right
		for (int i = f2Dto1D(0, number, sizeX); i < f2Dto1D(sizeX - 1, number, sizeX); i++)	//go through all tiles of selected line (except last tile) in order
		{
			//copy right tile
			copyTileContent(&laby[i], laby[i+1]);
		}
		//adding the previous extern tile to board
		copyTileContent(&laby[f2Dto1D(sizeX - 1, number, sizeX)], extern_tile);
		break;	
	case 2 :												//insert column top
		for (int i = f2Dto1D(number, sizeY - 1, sizeX); i > f2Dto1D(number, 0, sizeX); i = i - sizeX)		//go through all tiles of selected column (except first tile) in reverse order
		{
			//copy upward tile
			copyTileContent(&laby[i], laby[i - sizeX]);
		}
		//adding the previous extern tile to board
		copyTileContent(&laby[f2Dto1D(number, 0, sizeX)], extern_tile);
		break;
	case 3 :												//insert column bottom
		for (int i = f2Dto1D(number, 0, sizeX); i < f2Dto1D(number, sizeY - 1, sizeX); i = i + sizeX)		//go through all tiles of selected column (except last tile) in order
		{
			//copy downward tile
			copyTileContent(&laby[i], laby[i + sizeX]);
		}
		//adding the previous extern tile to board
		copyTileContent(&laby[f2Dto1D(number, sizeY - 1, sizeX)], extern_tile);
		break;
	default:												//Should never happen
		printf("move insert value impossible !!!!!\n");
		break;
	}
}

//function recursively marks the step on the available unmarked tiles adjacent to the tile entered in parameters (tileX ; tileY)
//laby is the labyrinth
//tileX and tileY are the coordinates of the current tile to test
//gx and gy are the coordinates of the goal
//sizeX and sizeY are the dimensions of the labyrinth
//steps is a pointer to an array of the steps for all tiles 
//currentStep is the step of the current tile (could have also been found from the steps array)
void recursivelyMarkSteps(tile *laby, int tileX, int tileY, int gx, int gy, int sizeX, int sizeY, int *steps, int currentStep) 
{
//	printf("launched recursivelyMarkSteps, tile %d;%d\n",tileX,tileY);
	if (!(tileX == gx && tileY == gy))	//test if tile isn't goal
	{
		//recursively try north path :
		if (0 <= (tileY - 1) && laby[tileY*sizeX+tileX].tileN == 0)	//test if north tile exists and there isn't a north wall on our tile
		{
			if (laby[(tileY-1)*sizeX+tileX].tileS == 0 && steps[(tileY-1)*sizeX+tileX] == -1)	//test if north tile doesn't have a south wall and the step count is still -1 on that tile
			{
//				printf("tile %d;%d : tileN is valid\n",tileX,tileY);
				steps[(tileY-1)*sizeX+tileX] = currentStep + 1;		//increment step of next tile (north tile)
				recursivelyMarkSteps(laby, tileX, tileY-1, gx, gy, sizeX, sizeY, steps, currentStep + 1);
			}
		}
		//recursively try east path :
		if (sizeX > (tileX + 1) && laby[tileY*sizeX+tileX].tileE == 0)	//test if east tile exists and there isn't a east wall on our tile
		{
			if (laby[tileY*sizeX+tileX+1].tileW == 0 && steps[tileY*sizeX+tileX+1] == -1)	//test if east tile doesn't have a south wall and the step count is still -1 on that tile
			{
//				printf("tile %d;%d : tileE is valid\n",tileX,tileY);
				steps[tileY*sizeX+tileX+1] = currentStep + 1;		//increment step of next tile (east tile)
				recursivelyMarkSteps(laby, tileX+1, tileY, gx, gy, sizeX, sizeY, steps, currentStep + 1);
			}
		}
		//recursively try south path :
		if (sizeY > (tileY + 1) && laby[tileY*sizeX+tileX].tileS == 0)	//test if south tile exists and there isn't a south wall on our tile
		{
			if (laby[(tileY+1)*sizeX+tileX].tileN == 0 && steps[(tileY+1)*sizeX+tileX] == -1)	//test if south tile doesn't have a south wall and the step count is still -1 on that tile
			{
//				printf("tile %d;%d : tileS is valid\n",tileX,tileY);
				steps[(tileY+1)*sizeX+tileX] = currentStep + 1;		//increment step of next tile (south tile)
				recursivelyMarkSteps(laby, tileX, tileY+1, gx, gy, sizeX, sizeY, steps, currentStep + 1);
			}
		}
		//recursively try west path :
		if (0 <= (tileX - 1) && laby[tileY*sizeX+tileX].tileW == 0)	//test if west tile exists and there isn't a west wall on our tile
		{
			if (laby[tileY*sizeX+tileX-1].tileE == 0 && steps[tileY*sizeX+tileX-1] == -1)	//test if west tile doesn't have a south wall and the step count is still -1 on that tile
			{
//				printf("tile %d;%d : tileW is valid\n",tileX,tileY);
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
	int steps[sizeY*sizeX];
	for (int i = 0; i < sizeX*sizeY; i++)
	{
		steps[i] = -1;
	}

	steps[py*sizeX + px] = 0;
	printf("launching recursivelyMarkSteps\n");
	//mark recursively steps :
	recursivelyMarkSteps(laby, px, py, gx, gy, sizeX, sizeY, steps, 0);
	printf("recursivelyMarkSteps exited\n");

	printf("steps array :\n");
	for (int i = 0; i < sizeY; i++)
	{
		for (int j = 0; j < sizeX; j++)
		{
			printf("%d ",steps[f2Dto1D(j, i, sizeX)]);
		}
		printf("\n");
	}

	// if no path has been found :
	if (steps[gy*sizeX + gx] == -1)
	{
		return NULL;
	}
	
	//allocate path array :
	int* path = malloc( (sizeof(int)) * (steps[gy*sizeX + gx] + 1) );	//allocate an array of size = number of steps to goal + 1
	path[0] = steps[gy*sizeX + gx] + 1;	//first element value is the size of the array

	printf("launching recursivelyAddPathValuesFromSteps\n");
	//get recursively path values from steps array to path array :
	recursivelyAddPathValuesFromSteps(gx, gy, sizeX, sizeY, steps, steps[gy*sizeX + gx], steps[gy*sizeX + gx], path);
	printf("recursivelyAddPathValuesFromSteps exited\n");
	return path;
}

int PlayMove(tile *laby, t_player *victor, int gx, int gy, int sizeX, int sizeY, tile *extern_tile, t_player *opponent)
{
	printf("PlayMove launched\n");
	int result;
/*	printf("launching testPathPlayerGoal\n");		//tempo comment
	int* path = testPathPlayerToGoal(laby, victor->x, victor->y, gx, gy, sizeX, sizeY);
	printf("testPathPlayerGoal exited\n");*/
//	if (path != NULL)		//not final yet, just for testing
//	{
//		move = {.insert = 0, .number = 7, .rotation = 3, .x = 0, .y = 0, .tileN = 1, .tileE = 0, .tileS = 0, .tileW = 0, .nextItem = 2};
//	}
/*	free(path);*/				//tempo commetn
	printf("insert type :\t");
	int insert;
	scanf("%d",&insert);
	printf("lign/column value :\t");
	int lign;
	scanf("%d",&lign);
	printf("rotation :\t");
	int rot;
	scanf("%d",&rot);

	t_move move = {.insert = insert, .number = lign, .rotation = rot, .x = gx, .y = gy, .tileN = 1, .tileE = 0, .tileS = 0, .tileW = 0, .nextItem = 1};
	int posx = victor->x;
	int posy = victor->y;
	updateGame(laby, move, sizeX, sizeY, extern_tile, victor, opponent);

	printf("launching testPathPlayerGoal\n");	//tempo
	int* path = testPathPlayerToGoal(laby, posx, posy, gx, gy, sizeX, sizeY);
	printf("testPathPlayerGoal exited\n");
	free(path);
	result = sendMove(&move);
	printf("%d\n",result);
	return result;
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

t_move findBestMoveV1(tile *laby, int sizeX, int sizeY, tile externTile, t_player activPlayer)
{
	//Creating a new board with the contents of laby
	tile nLaby[sizeY*sizeX];
	cpyBoard(nLaby, laby, sizeX, sizeY);

	t_possibleMove bestMove = {.activPlayerDist = 999};		//declared with an impossibly high best distance found to be replaced at first iteration
	t_possibleMove cMove;									//current move being tested
	t_move moveToTest;

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
				for (int n = 1; n < sizeX; n = n + 2)	//Number
				{
					cMove.number = n;
					moveToTest.number = n;

					updateLaby(nLaby, i, n, r, sizeX, sizeY, externTile)

					findCoordsGoal(, activPlayer.item);
				}
			}
			else
			{
				for (int n = 1; n < sizeY; n = n + 2)	//Number
				{
					/* code */
				}
			}			
		}
	}
	

}

int main(void)
{
	//connection to server
	printf("connection to server :");
	connectToServer("172.105.76.204", 5678, "VFtest");
	printf(" success\n");

	int sizeX;		//labyrinth size
	int sizeY;		//labyrinth size

	char name[50] = "test";

	printf("waiting for labyrinth :");
	waitForLabyrinth("TRAINING DONTMOVE timeout=600 seed=177013 start=0", name, &sizeX, &sizeY);
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
//	int lenLaby = sizeX*sizeY;

	for (int i = 0; i < sizeX*sizeY; i++)
	{
		tile tileToAdd = {.x = i%sizeX, .y = i/sizeX, .tileN = LabyrinthReceivedTab[i*5+0], .tileE = LabyrinthReceivedTab[i*5+1], .tileS = LabyrinthReceivedTab[i*5+2], .tileW = LabyrinthReceivedTab[i*5+3], .tileItem = LabyrinthReceivedTab[i*5+4]};
		Laby[i] = tileToAdd;
	}
// testing:
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[0].x,Laby[0].y,Laby[0].tileN,Laby[0].tileE,Laby[0].tileS,Laby[0].tileW,Laby[0].tileItem);	//(0;0)
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[1].x,Laby[1].y,Laby[1].tileN,Laby[1].tileE,Laby[1].tileS,Laby[1].tileW,Laby[1].tileItem);	//(1;0)
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[sizeX].x,Laby[sizeX].y,Laby[sizeX].tileN,Laby[sizeX].tileE,Laby[sizeX].tileS,Laby[sizeX].tileW,Laby[sizeX].tileItem);	//(0;1)
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[sizeX+1].x,Laby[sizeX+1].y,Laby[sizeX+1].tileN,Laby[sizeX+1].tileE,Laby[sizeX+1].tileS,Laby[sizeX+1].tileW,Laby[sizeX+1].tileItem);	//(1;1)
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[(sizeY-1)*sizeX].x,Laby[(sizeY-1)*sizeX].y,Laby[(sizeY-1)*sizeX].tileN,Laby[(sizeY-1)*sizeX].tileE,Laby[(sizeY-1)*sizeX].tileS,Laby[(sizeY-1)*sizeX].tileW,Laby[(sizeY-1)*sizeX].tileItem);	//(0;sizeY-1)
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[sizeX-1].x,Laby[sizeX-1].y,Laby[sizeX-1].tileN,Laby[sizeX-1].tileE,Laby[sizeX-1].tileS,Laby[sizeX-1].tileW,Laby[sizeX-1].tileItem);	//(1;1)
	printf("tile %d;%d is %d %d %d %d %d\n",Laby[(sizeY-1)*sizeX+(sizeX-1)].x,Laby[(sizeY-1)*sizeX+(sizeX-1)].y,Laby[(sizeY-1)*sizeX+(sizeX-1)].tileN,Laby[(sizeY-1)*sizeX+(sizeX-1)].tileE,Laby[(sizeY-1)*sizeX+(sizeX-1)].tileS,Laby[(sizeY-1)*sizeX+(sizeX-1)].tileW,Laby[(sizeY-1)*sizeX+(sizeX-1)].tileItem);	//(0;sizeY-1)

	//start game
	while (1)
	{
		printLabyrinth();
		if (turn == 0)			//player turn
		{
			int xdest = 7;
			int ydest = 7;
			printf("goal coords :\t");
			scanf("%d%d",&xdest,&ydest);
			printf("our turn, launching PlayMove\n");
			int playMoveResult = PlayMove(Laby, &victor, xdest, ydest, sizeX, sizeY, &extern_tile, &opponent);
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
			int ennemyMoveResult = getMove(&ennemy_move);
			updateGame(Laby, ennemy_move, sizeX, sizeY, &extern_tile, &opponent, &victor);
			if(ennemyMoveResult==0){
				printf("normal opponent move\n");
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