# Jeu du Labyrinth

main.c et Makefile Par Victor Fleiser

## Description : 

_Objectif :_

	L'objectif était de créer un programme permettant de jouer automatiquement un certain nombre de parties au jeu labyrinth en ligne utilisant les ressources données (consignes, client.c, client.h, labyrinthAPI.c, labyrinthAPI.h).

_Utilisation :_

	- Utiliser le Makefile pour créer l'executable "main"
	- Pour choisir le nombre de parties à jouer, il faut changer la valeur assignée à numberOfGamesLeft dans la fonction main().
	- Pour choisir le type de partie à jouer, il faut changer les arguments dans l'appel de la fonction waitForLabyrinth() dans playGame().

## Contenu :

Le programme écrit est contenu entièrement dans main.c et contient les structures et fonctions suivantes :

_Structures :_

	- tile : correspond à la structure d'une case, contient : ses coordonnées, ses murs, et son objet

	- t_player : correspond à un joueur, contient : position, prochain objectif
	
	- t_possibleMove : correspond à un move qui est testé dans la fonction findBestMoveV1(), contient des champs similaires à t_move, mais sans l'objectif suivant et la case externe; possède des nouveaux champs pour la distance des joueurs à leur buts.

_Fonctions principales :_

	- main : connecte/déconnecte le programme, lancera numberOfGamesLeft parties d'affilée avec la fonction playGame(), calculera également le nombre de movements moyens éffectués.
	
	- playGame : s'occupe de jouer une partie du début à la fin : initiallise les variables, tableaux ; puis entre une boucle while itérant chaque tour jusqu'à la fin de la partie.
	
	- playMoveV2 : trouve le meilleur move possible avec findBestMoveV1(), puis met à jour le labyrinth avec et envoie le move au serveur.
	
	- findBestMoveV1 : trouve le meilleur move possible selon les critères suivants : le critère le plus important est la distance entre la destination du joueur et son but, le critère secondaire est la distance entre l'ennemi et son but.
	
	- testPathPlayerToGoalV2 : renvoie la distance 
	minimale entre le joueur après movement et le but en fonction du mouvement possible éffectué par le joueur, utilise la fonction recursivelyMarkDistance() pour calculer ces distances.
	
	- recursivelyMarkDistance : de maninère récursive : va sur chaque case accessible par le joueur et marque sa distance au but dans un tableau pointé en paramètre.
	
	- updateLaby : met à jour un tableau labyrinth avec le move passé en paramètre (utilisé dans la fonction findBestMoveV1() ).
	
	- updateGame : met à jour le tableau labyrinth ainsi que les joueurs et case externe avec le move passé en paramètre.


_Fonctions outils :_

	- findCoordsGoal : renvoie la position du but dans le tableau passé en paramètre.

	- cpyBoard : copie le contenu d'un tableau source vers un tableau destination.
	
	- calcDist : calcule la distance entre deux cases passées en paramètre.
	
	- copyTileContent : copie le contenu de la case source à la case destination, toutes deux en paramètres.
	
	- rotateTile : tourne la case passée en paramètre le nombre de fois demandé en paramètre.
	
	- f1Dto2Dy : renvoie la composante y des coordonnées 2D de la case correspondante à la case en coordonnées 1D passée en paramètre.
	
	- f1Dto2Dx : renvoie la composante x des coordonnées 2D de la case correspondante à la case en coordonnées 1D passée en paramètre.
	
	- f2Dto1D : renvoie la coordonnée 1D de la case correspondante à la case en coordonnées 2D passées en paramètre.

