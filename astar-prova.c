#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <omp.h>

#define DIM 2000				// lateral dimension of the map
#define CONNECTIVITY 8			// degree of freedom - it can be 4 or 8
#define OBSTACLES 20			// percentage of obstacles
#define ALLOC 10				// dimension used for dynamic vector allocation 
#define ARR_MAX 2000			// half of maximum dimension of an array to be printed

typedef struct {
	int row, col;				// row and column of a cell
	double f;					// total cost of a cell
	double g;					// distance (cost) between a cell and the starting point
	double h;					// heuristic - estimated distance (cost) between a cell and the goal point
	int parentRow, parentCol;	// row and column of the parent cell 
	bool isInClosedSet, isInOpenSet;
} Cell;

// check the values defined with #define directives
bool checkDefine(){
	if (DIM <= 0){
		printf("\n%d is not a valid dimension.\n", DIM);
		return false;
	} else if (CONNECTIVITY != 4 && CONNECTIVITY != 8){
		printf("\n%d is not a valid connectivity.\n", CONNECTIVITY);
		return false;
	} else if (OBSTACLES < 0 || OBSTACLES > 100){
		printf("\n%d is not a valid obstacle percentage.\n", OBSTACLES);
		return false;
	} else if (ALLOC <= 0){
		printf("\n%d is not a valid value for reallocation.\n", ALLOC);
		return false;
	} else if (ARR_MAX <= 0){
		printf("\n%d is not a valid string size for the printing.\n", ARR_MAX);
		return false;
	} else {
		return true;
	}
}

// check if a cell is inside the map
bool check_position (int cell[]) {
	if (cell[0] >= 0 && cell[0] < DIM && cell[1] >= 0 && cell[1] < DIM)
		return true;
	else
		return false; 
}

// check whether a cell is free or not
bool is_free (int cell[], bool map[]) {
	if (map[cell[0]*DIM+cell[1]] == true)
		return true;
	else
		return false;
}

// check if the goal point has been reached
bool is_goal (int start[], int goal[]) {
	if (start[0] == goal[0] && start[1] == goal[1])
		return true;
	else
		return false;
}

// check the correctness of the start and goal cells
bool check (int start[], int goal[], bool map[]) {
	if (!(check_position(start) && check_position(goal))) {
		printf("ERROR: start or goal point is out of the map!\n");
		return false;
	}

	if (!(is_free(start, map) && is_free(goal, map))) {
		printf("ERROR: start or goal point is not free!\n");
		return false;
	}

	if (is_goal(start, goal)) {
		printf("ERROR: start and goal points coincide!\n");
		return false;
	}
	return true;
}

// fills the map at the beginning, with random values
bool fillMap(bool map[], int start[], int goal[]){
	printf("Opening file with DIM = %d and OBSTACLES = %d...\n", DIM, OBSTACLES);

	FILE *fp;
	
	int free_cell_num = 0, i = 0;
	int c;
	char fname[40];
	sprintf(fname,"./maps/map-dim%d-obst%d",DIM,OBSTACLES);
	
	fp = fopen(fname,"r");
	
	if (fp == NULL){
		printf("Errore nell'apertura del file.\n");
		exit(0);
	} else {
		printf("Filling map... ");
		double startT = 0; double endT = 0;
		startT = omp_get_wtime();
		
		while(!feof(fp)){
			fscanf(fp,"%d ",&c);
			if(c){
				map[i] = true;
				free_cell_num++;
			} else {
				map[i] = false;
			}
			i++;
		}
		
		// start and goal points assumed as always free
		map[start[0]*DIM+start[1]] = true;
		map[goal[0]*DIM+goal[1]] = true;
		
		endT = omp_get_wtime();
		printf("Map %dx%d filled in %f seconds with %d free cells.\n", DIM, DIM, endT-startT, free_cell_num);
		return 1;
	}
}

// returns the position of a cell in the vector "arrayCells[]" used in the "search()" function
int calculatePos(int cell[]){
	return cell[0]*DIM + cell[1];
}

// initialization of every cell
void initCells(Cell arrayCells[], int start[], int goal[]){	
	int i,j,pos;
	double startT,endT;
	startT = omp_get_wtime();
//	#pragma omp parallel for private(i,j) shared(arrayCells,pos)
	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			pos = i*DIM + j;
			arrayCells[pos].row = i;
			arrayCells[pos].col = j;
			arrayCells[pos].f = 200000.0;		// very high number
			arrayCells[pos].g = 100000.0;		// very high number
			arrayCells[pos].h = 100000.0;		// very high number
			arrayCells[pos].parentRow = -1;		// invalid position
			arrayCells[pos].parentCol = -1;		// invalid position
			arrayCells[pos].isInClosedSet = false;
			arrayCells[pos].isInOpenSet = false;
		}
	}
	endT = omp_get_wtime();
	printf("InitCells ha impiegato %f secondi.\n", endT-startT);

	// initialization of the starting cell
	int posS = calculatePos(start);
	arrayCells[posS].f = 0.0;
	arrayCells[posS].g = 0.0;
	arrayCells[posS].h = 0.0;
	arrayCells[posS].parentRow = start[0];
	arrayCells[posS].parentCol = start[1];
}

// returns the Euclidean distance between two cells
double distance (Cell a, Cell b) { 
	return ((double)sqrt((a.row - b.row)*(a.row - b.row) + (a.col - b.col)*(a.col - b.col)));
}

// check if a neighbor is valid (i.e., if it is inside the map) and free 
bool check_a_neighbor(int deltaRow, int deltaCol, int cell[], bool map[]){
	int neighbor[2];
	neighbor[0] = cell[0] + deltaRow;
	neighbor[1] = cell[1] + deltaCol;
	if((check_position(neighbor)) && (is_free(neighbor, map)))
		return true;
	else
		return false;
}

// evaluation of the best parent (in terms of g) for the cell thisCell[].
int chooseBestParent(Cell arrayCells[], bool map[], int thisCell[], int bestParent[]){
	int thisCellPos = calculatePos(thisCell);
	int deltaRow, deltaCol;
	int bpPos = calculatePos(bestParent);					
	int currentParent[2];
	int cpPos;
	for (deltaRow=-1; deltaRow<=1; deltaRow++){
		for (deltaCol=-1; deltaCol<=1; deltaCol++){
			if (deltaRow != 0 || deltaCol != 0){	// this excludes the cell itself from the evaluation
				if (CONNECTIVITY == 8 || (deltaRow == 0 || deltaCol == 0)){		// this checks the connectivity and works consequently
					if (check_a_neighbor(deltaRow, deltaCol, thisCell, map)) {
						currentParent[0] = thisCell[0]+deltaRow;
						currentParent[1] = thisCell[1]+deltaCol;
						cpPos = calculatePos(currentParent);
						if(arrayCells[cpPos].g < arrayCells[bpPos].g){
							bestParent[0] = currentParent[0];
							bestParent[1] = currentParent[1];
							bpPos = calculatePos(bestParent);
							arrayCells[thisCellPos].parentRow = currentParent[0];
							arrayCells[thisCellPos].parentCol = currentParent[1];
						}
					}
				}
			}
		}
	}
	return bpPos;
}

// NON SERVE PIU!!!!!!!!!!!!

// print of the map with indication of start point, goal point and obstacles
void printOnlyMap(bool map[], int posStart, int posGoal){
	int ripet = DIM / ARR_MAX;
	int remain = DIM % ARR_MAX;
	int r,c,n;
	char *row = malloc(2*ARR_MAX);
	
	printf("\n\n");
	for (r=0; r<DIM; r++){
		for (n=0; n<ripet; n++){
			for (c=0; c<ARR_MAX; c++){
				int posCell = r*DIM+n*ARR_MAX+c;
				if (map[posCell]){
					if (posCell == posStart)
						row[2*c] = 'S';		// start point
					else if (posCell == posGoal)
						row[2*c] = 'G';		// goal point
					else
						row[2*c] = '.';		// free cell
				} else {
					row[2*c] = 'X';			// obstacle
				}
				row[2*c+1] = ' ';
			}
			row[2*ARR_MAX] = '\0';
			printf("%s", row);
		}
		for (c=0; c<remain; c++){
			int posCell = r*DIM+n*ARR_MAX+c;
			if (map[posCell]){
				if (posCell == posStart)
					row[2*c] = 'S';		// start point
				else if (posCell == posGoal)
					row[2*c] = 'G';		// goal point
				else
					row[2*c] = '.';		// free cell
			} else {
				row[2*c] = 'X';			// obstacle
			}
			row[2*c+1] = ' ';
		}
		row[2*remain] = '\0';
		printf("%s\n", row);
	}
	free(row);
}

// print of the best path found
void printPath(Cell arrayCells[], int bestPath[], int bestPathSize, bool map[]){
	printf("\nGoal reached through %d intermedium cells. Path length %f over minimum distance %f (+ %.2f \%%).\n\n", bestPathSize-2, arrayCells[bestPath[0]].f, distance(arrayCells[bestPath[bestPathSize-1]], arrayCells[bestPath[0]]), ((arrayCells[bestPath[0]].f / distance(arrayCells[bestPath[bestPathSize-1]], arrayCells[bestPath[0]])) - 1)*100);
	int ripet = DIM / ARR_MAX;
	int remain = DIM % ARR_MAX;
	int r,c,n;
	char *row = malloc(2*ARR_MAX*sizeof(bool));
	
	double t1 = omp_get_wtime();
	
	FILE *fp;
	
	char fname[40];
	sprintf(fname,"./paths/path-dim%d-obst%d",DIM,OBSTACLES);
	
	fp = fopen(fname,"w");
	if (fp == NULL){
		printf("Errore nell'apertura del file.\n");
		exit(1);
	} else {
		printf("Stampando il percorso su file... ");
		
		for (r=0; r<DIM; r++){
			for (n=0; n<ripet; n++){
				for (c=0; c<ARR_MAX; c+=1){
					int posCell = r*DIM+n*ARR_MAX+c;
					if (!map[posCell]){
						row[2*c] = 'X';
					} else {
						bool isPath = false;
						for (int b=0; b<bestPathSize; b++){
							if(arrayCells[posCell].row == arrayCells[bestPath[b]].row && arrayCells[posCell].col == arrayCells[bestPath[b]].col)
								isPath = true;
						}
						if(isPath){
							if(arrayCells[posCell].row == arrayCells[bestPath[0]].row && arrayCells[posCell].col == arrayCells[bestPath[0]].col){
								row[2*c] = 'G';		// goal point
							} else if(arrayCells[posCell].row == arrayCells[bestPath[bestPathSize-1]].row && arrayCells[posCell].col == arrayCells[bestPath[bestPathSize-1]].col){
								row[2*c] = 'S';		// starting point
							} else row[2*c] = 'O';		// point of the path
						} else
							row[2*c] = '.';		// free
					}
					row[2*c+1] = ' ';
				}
				row[2*ARR_MAX] = '\0';
				fprintf(fp, "%s", row);
			}
			for (c=0; c<remain; c++){
				int posCell = r*DIM+n*ARR_MAX+c;
				if (!map[posCell]){
					row[2*c] = 'X';
				} else {
					bool isPath = false;
					for (int b=0; b<bestPathSize; b++){
						if(arrayCells[posCell].row == arrayCells[bestPath[b]].row && arrayCells[posCell].col == arrayCells[bestPath[b]].col)
							isPath = true;
					}
					if(isPath){
						if(arrayCells[posCell].row == arrayCells[bestPath[0]].row && arrayCells[posCell].col == arrayCells[bestPath[0]].col){
							row[2*c] = 'G';		// goal point
						} else if(arrayCells[posCell].row == arrayCells[bestPath[bestPathSize-1]].row && arrayCells[posCell].col == arrayCells[bestPath[bestPathSize-1]].col){
							row[2*c] = 'S';		// starting point
						} else row[2*c] = 'O';		// point of the path
					} else
						row[2*c] = '.';		// free
				}
				row[2*c+1] = ' ';
			}
			row[2*remain] = '\0';
			fprintf(fp, "%s\n", row);
		}
		
		printf("Percorso stampato su file. ");
		fclose(fp);
		printf("File chiuso correttamente.\n\n");
	}
	
	double t2 = omp_get_wtime();
	printf("Tempo per la stampa su file: %f\n", t2-t1);
	
	free(row);
}

// explicits the result of the search - if a path is found, then it is printed, otherwise a message is printed that warns of the unreachability of the goal point  
void endSearch(bool foundPath, Cell arrayCells[], int bestPath[], int bestPathSize, bool map[], int posStart, int posGoal) {
	if(foundPath) {
		printPath(arrayCells, bestPath, bestPathSize, map);
	} else {
		printOnlyMap(map, posStart, posGoal);
		printf("\nGoal not reachable!\n");
	}
}

// free all dynamic vectors
void freeAll(int openSet[], int closedSet[], int path[], int bestPath[]){
	free(openSet);
	free(closedSet);
	free(path);
	free(bestPath);
}

// main part of the algorithm - here all operations are carried out to find the best path 
void search (bool map[], int start[], int goal[]) {	

	double ss = omp_get_wtime();
	
	double somma_time = 0, tot_time = 0, t1 = 0, t2 = 0, t3 = 0;
	double st=0, et=0;
	double s1=0, e1=0, s2=0, e2=0, s3=0, e3=0;


	printf("Filling arrayCells... ");
	Cell *arrayCells;
	arrayCells = (Cell *)malloc(DIM*DIM*sizeof(Cell));

	// check if the allocation is done correctly 
	if(arrayCells == NULL) {
		printf("\nFailed allocation.\n");
		return;
	}

	// initialization of each cell
	initCells(arrayCells, start, goal);
	
	int posS = calculatePos(start);
	int posG = calculatePos(goal);

	// Initialization of 4 vectors, i.e., openSet, closedSet, path, bestPath
	
	int *openSet;			// priority queue - it contains all the cells that step by step will have to be considered 
	int allocOpen = ALLOC;
	openSet = (int*)malloc(sizeof(int)*allocOpen);
	openSet[0] = posS;		// the first cell in the open set is the starting cell
	int openSetSize = 1;

	int *closedSet;			// it contains all the cells already considered 
	int allocClosed = ALLOC;
	closedSet = (int*)malloc(sizeof(int)*allocClosed);
	int closedSetSize = 0;
	
	bool foundPath = false;
	
	int *path;				// path between the starting point and the goal point
	int allocPath = ALLOC;
	path = (int*)malloc(sizeof(int)*allocPath);
	int pathSize = 0;
	
	int *bestPath;
	bestPath = (int*)malloc(sizeof(int)*allocPath);
	int bestPathSize = 0;

	printf("Searching the best path...\n");
	
	int i, best, c[2];		// c is the current cell
	bool isThereBest;
	double tmpF;

	while (1) {
		best = 0;
		isThereBest = false;
		
		s3= omp_get_wtime();
		
		// scan the open set to find the new best cell
		
		  	// QUESTO E' L'ORIGINALE
/*			  	
		for (i = 0; i < openSetSize; i++) {
			tmpF = arrayCells[openSet[i]].f;
			if (tmpF <= arrayCells[openSet[best]].f) {
				if ((foundPath && tmpF <= arrayCells[bestPath[0]].f) || !foundPath){
					isThereBest = true;
					best = i;
				}
			}
		}
*/
		#pragma omp parallel
		{
			int best_private = 0;	// initial assumption: the cell having the lowest value of f is in the first position of the open set
			
			#pragma omp for nowait
			for (int i = 0; i < openSetSize; i++) {
				double tmpF = arrayCells[openSet[i]].f;
				if (tmpF <= arrayCells[openSet[best_private]].f) {
					if ((foundPath && tmpF <= arrayCells[bestPath[0]].f) || !foundPath){
						isThereBest = true;
						best_private = i;
					}
				}
			}
			
			#pragma omp critical
			{
				if(arrayCells[openSet[best_private]].f < arrayCells[openSet[best]].f){
					best = best_private;
				}
			}
		}
		
		
		
		
		
		
		
		e3= omp_get_wtime();
		t3 += e3-s3;

		// condition that terminates the algorithm 		
		if((foundPath && !isThereBest) || openSetSize == 0){
			
			double end = omp_get_wtime();
			
		//	endSearch(foundPath, arrayCells, bestPath, bestPathSize, map, posS, posG);
			printf("Tempo nel ciclo for esterno: %f\n", tot_time);
			printf("Tempo nel T1: %f\nTempo nel T2: %f\nTempo nel T3: %f", t1, t2, t3);
			printf("\nTempo nel search(): %f sec\n", end-ss);

			freeAll(openSet, closedSet, path, bestPath);
			return;
		}
		
		c[0] = arrayCells[openSet[best]].row;
		c[1] = arrayCells[openSet[best]].col;
		
		int posC = calculatePos(c);		
		if(posC == posS){
			arrayCells[posS].h = distance(arrayCells[posS], arrayCells[posG]);
			arrayCells[posS].f = arrayCells[posS].h;
		}
		
		s2 = omp_get_wtime();
		
		// remove the current cell from the open set
		
		for (int i = best; i < openSetSize; i++) {
			openSet[i] = openSet[i+1];
		}
		openSetSize--;
		
		arrayCells[posC].isInOpenSet = false;
		
		arrayCells[posC].isInClosedSet = true;
		
		e2 = omp_get_wtime();
		t2 += e2-s2;
		
				
		if (is_goal(c, goal)) {					// check whether the current cell is the goal point or not
			int thisCell[2] = {c[0], c[1]};		// thisCell = c
			int thisCellPos = posC;
							
			path[0] = thisCellPos;				// backward path (from goal to start)
			pathSize = 1;
			
			// these instructions are executed every time a cell has a parent (the loop stops when the starting cell is evaluated, whose parent is the cell itself) 
			while ((arrayCells[thisCellPos].parentRow != -1) && (arrayCells[thisCellPos].row != arrayCells[thisCellPos].parentRow || arrayCells[thisCellPos].col != arrayCells[thisCellPos].parentCol)) {
				
				// possible reallocation of Path and BestPath vectors 
				if(pathSize >= allocPath){
					allocPath += ALLOC;
					path = (int*)realloc(path, allocPath*sizeof(int));
					bestPath = (int*)realloc(bestPath, allocPath*sizeof(int));
				}
				
				// choose the best parent for thisCell
				int bestParent[2] = {arrayCells[thisCellPos].parentRow, arrayCells[thisCellPos].parentCol};
				int bpPos = chooseBestParent(arrayCells, map, thisCell, bestParent);
				
				// thisCell is updated to the best parent
				thisCellPos = bpPos;
				bestParent[0] = arrayCells[bpPos].row;
				bestParent[1] = arrayCells[bpPos].col;
				thisCell[0] = bestParent[0];
				thisCell[1] = bestParent[1];
				
				path[pathSize] = bpPos;			// add the parent cell to the path
				pathSize++;
			}
			
			// updates BestPath if it is the first path found OR if the new path is better than the previous one 
			if(!foundPath || (foundPath && arrayCells[path[0]].f < arrayCells[bestPath[0]].f)) {
				for(int i = 0; i < pathSize; i++){
					bestPath[i] = path[i];
				}
				bestPathSize = pathSize;
			}
			
			foundPath = true;

			float pathF = arrayCells[bestPath[0]].f;
			for(int i = 0; i < openSetSize; i++){
				if (arrayCells[openSet[i]].f >= pathF){
					
					arrayCells[openSet[i]].isInClosedSet = true;		// AGGIUNTO
					
					arrayCells[openSet[i]].isInOpenSet = false;		// AGGIUNTO
					
					for (int j = i; j < openSetSize; j++){					
						// remove the cell from the OpenSet
						openSet[j] = openSet[j+1];
					}
					openSetSize--;

				}
			}
		}
		
		int numNeighbors = 0;
		int neighbor[2];			// row and column of a neighbor
		int tmp[CONNECTIVITY];		// vector that will contain all the possible neighbors of c
		
		// for cell "c", each possible neighbor is checked
		int deltaRow, deltaCol;
		for (deltaRow=-1; deltaRow<=1; deltaRow++){
			for (deltaCol=-1; deltaCol<=1; deltaCol++){
				if (deltaRow != 0 || deltaCol != 0){	// this excludes the cell itself from the evaluation
					if (CONNECTIVITY == 8 || (deltaRow == 0 || deltaCol == 0)){		// this check the connectivity and works consequently
						if (check_a_neighbor(deltaRow, deltaCol, c, map)) {
							neighbor[0] = c[0] + deltaRow;
							neighbor[1] = c[1] + deltaCol;
							tmp[numNeighbors] = neighbor[0]*DIM + neighbor[1];
							numNeighbors++;
						}
					}
				}
			}
		}
	
		int neighbors[numNeighbors];
		for (int i=0; i<numNeighbors; i++){
			neighbors[i] = tmp[i];
		}
				
		int j, k, posN;
		double tmpG;
		
		
		// loop for checking every neighbor of the current cell
		
		st = omp_get_wtime();
		
		for (int i = 0; i < numNeighbors; i++) {
			
			neighbor[0] = arrayCells[neighbors[i]].row;
			neighbor[1] = arrayCells[neighbors[i]].col;
			posN = calculatePos(neighbor);
	
			if(!(arrayCells[posN].isInClosedSet)) {
				
				tmpG = arrayCells[posC].g + distance(arrayCells[posC], arrayCells[posN]);					
				
				
				if(c[0] == start[0] && c[1] == start[1]){ 	// check if I am at the beginning, i.e., if the cell is the starting point
					
					// possible reallocation of OpenSet vector
					if(openSetSize >= allocOpen){
						allocOpen += ALLOC;
						openSet = (int*)realloc(openSet, allocOpen*sizeof(int));
					}
					
					// add "neighbor" to the open set
					openSet[openSetSize] = posN;
					arrayCells[posN].g = tmpG;
					openSetSize++;
					
					arrayCells[posN].isInOpenSet = true;
					
				} else {		// general case, i.e., I am not at the begininng (the cell is NOT the starting point)
									
					// check if the neighbor is already in the open set - if it is NOT, a new cell was discovered
					if(arrayCells[posN].isInOpenSet) {
						if (tmpG < arrayCells[posN].g) {
							arrayCells[posN].g = tmpG;
						}	
					}
						
					// check if the neighbor is already in the open set - if it is NOT, a new cell was discovered
					if (((! arrayCells[posN].isInOpenSet) && (!arrayCells[posN].isInClosedSet )) || (openSetSize == 0)) {
					
						// possible reallocation of OpenSet vector
						if(openSetSize >= allocOpen){
							allocOpen += ALLOC;
							openSet = (int*)realloc(openSet, allocOpen*sizeof(int));
						}
						
						// add "neighbor" to the open set
						if(!foundPath || (foundPath && tmpG+distance(arrayCells[posN], arrayCells[posG]) < arrayCells[bestPath[0]].f)){
							arrayCells[posN].g = tmpG;
							openSet[openSetSize] = posN;
							openSetSize++;
							
							arrayCells[posN].isInOpenSet = true;
						}
					}
				}

				arrayCells[posN].h = distance(arrayCells[posN], arrayCells[posG]);
				arrayCells[posN].f = arrayCells[posN].g + arrayCells[posN].h;
				arrayCells[posN].parentRow = c[0];
				arrayCells[posN].parentCol = c[1];
					
			}
		}
		
		et = omp_get_wtime();
		tot_time += et-st;
		
	}
}

int main () {

	double st = omp_get_wtime();
	
	//omp_set_num_threads(1);			//	ATTENZIONE A QUESTO VALORE
	
	if (!checkDefine()){
		return 0;
	}

	bool *map;
	map = (bool *)malloc(DIM*DIM*sizeof(bool));
	if(map == NULL) {
		printf("\nFailed allocation.\n");
		return 0;
	}
	
	int start[] = {0, DIM-1};
	int goal[] = {DIM-1, 0};
	
	fillMap(map, start, goal);
	
	if (check(start, goal, map)) {
		// execute the algorithm
		
		search(map, start, goal);
		
		double end = omp_get_wtime();
		printf("Tempo totale programma: %f\n", end-st);
		
	}
	else {
		return 0;
	}
	return 1;
}
