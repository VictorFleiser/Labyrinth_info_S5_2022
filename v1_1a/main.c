#include <stdio.h>
#include "labyrinthAPI.h"




int main(void)
{

	int sizeX = 5;
	int sizeY = 5;

	int* tab[125];

	char* name;
	int* tileN, tileE, tileS, tileW, tileItem;

	//connection to server
	printf("connection to server :");
	connectToServer("172.105.76.204", 5678, "VFtest");
	printf(" success\n");

	printf("waiting for labyrinth\n");
	waitForLabyrinth("TRAINING DONTMOVE timeout=10 seed=69420 start=1", name, &sizeX, &sizeY);

	printf("getting labyrinth\n");
	getLabyrinth(&tab, &tileN, &tileE, &tileS, &tileW, &tileItem);

	printLabyrinth();

	//disconnecting from server
	printf("disconnecting from server\n");
	closeConnection();


	printf("\n%d\n",tileItem);


	return 1;
}

