#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <omp.h>

#define DIM 500					// Dimension of rows and columns of the map
#define OBSTACLES 10			// Percentage of obstacles in the map
#define CONNECTIVITY 8			// Degree of freedom - must be 8
#define ALLOC 100				// Dimension used for dynamic vector allocation 
#define ARR_MAX 2000			// Half of maximum dimension of an array to be printed (depends on the hardware)

typedef struct {
	int row, col;				// Row and column of a cell
	double f;					// Total cost of a cell
	double g;					// Distance (cost) between a cell and the starting point
	double h;					// Heuristic - estimated distance (cost) between a cell and the goal point
	int parentRow, parentCol;	// Row and column of the parent cell 
} Cell;

/* Check if the values defined via the #define directives are correct. If they are, it is returned true, otherwise an error
message is reported based on the first problem encountered and it is returned false.*/
bool checkDefine(){
	if (DIM <= 0){
		printf("\n%d is not a valid dimension.\n", DIM);
		return false;
	} else if (CONNECTIVITY != 8){
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

/* Check if a cell is inside the map. If it is, it is returned true, otherwise false. */
bool check_position (int cell[]) {
	if (cell[0] >= 0 && cell[0] < DIM && cell[1] >= 0 && cell[1] < DIM)
		return true;
	else
		return false; 
}

/* Check if a cell is free or not. If it is, it is returned true, otherwise false. */
bool is_free (int cell[], bool map[]) {
	if (map[cell[0]*DIM+cell[1]] == true)
		return true;
	else
		return false;
}

/* Check if the goal point has been reached. If it is, it is returned true, otherwise false. */
bool is_goal (int start[], int goal[]) {
	if (start[0] == goal[0] && start[1] == goal[1])
		return true;
	else
		return false;
}

/* Check the correctness of the starting point and the goal point, i.e., if both are within the map, if they are free
and if their positions do not coincide. If everything is ok, it is returned true, otherwise an error message is reported
based on the first problem encountered and it is returned false. */
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

/* This function has the purpose of building the map, obtaining the values of interest from the "map-dim%d-obst%d" file
present in the "maps" folder. This file contains several 0s and 1s, where the value 0 indicates that the cell is occupied
(i.e., there is an obstacle), while the value 1 indicates that the cell is free. The function therefore tries to open the
file for reading and, if it succeeds, inserts true values into the array map[] whenever it encounters 1s and false values
whenever it encounters 0s. In case of an error in opening an error message is shown. */
bool fillMap(bool map[], int start[], int goal[]){
	printf("Opening file with DIM = %d and OBSTACLES = %d...\n", DIM, OBSTACLES);

	FILE *fp;
	
	int free_cell_num = 0, i = 0;
	int c;
	char fname[40];
	sprintf(fname,"./maps/map-dim%d-obst%d",DIM,OBSTACLES);
	
	fp = fopen(fname,"r");
	
	if (fp == NULL){
		printf("Error while opening the input file.\n");
		exit(0);
	} else {
		printf("Filling map... ");
		for(i=0; i<DIM*DIM; i++){
			if(!feof(fp)){
				fscanf(fp,"%d ",&c);
				if(c){
					map[i] = true;
					free_cell_num++;
				} else {
					map[i] = false;
				}
			}
		}
		
		// Start and goal points assumed as always free
		map[start[0]*DIM+start[1]] = true;
		map[goal[0]*DIM+goal[1]] = true;
		
		printf("Map %dx%d filled with %d free cells.\n", DIM, DIM, free_cell_num);
		fclose(fp);
		return 1;
	}
}

/* This function returns the position of a cell in the vector "arrayCells[]" used in the "search()" function. */
int calculatePos(int cell[]){
	return cell[0]*DIM + cell[1];
}

/* Initialization of every cell. */
void initCells(Cell arrayCells[], int start[], int goal[]){	
	int i,j,pos;
	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			pos = i*DIM + j;
			arrayCells[pos].row = i;
			arrayCells[pos].col = j;
			arrayCells[pos].f = 200000.0;		// Very high number
			arrayCells[pos].g = 100000.0;		// Very high number
			arrayCells[pos].h = 100000.0;		// Very high number
			arrayCells[pos].parentRow = -1;		// Invalid position
			arrayCells[pos].parentCol = -1;		// Invalid position
		}
	}
	printf("InitCells completed.\n");

	// Initialization of the starting cell
	int posS = calculatePos(start);
	arrayCells[posS].f = 0.0;
	arrayCells[posS].g = 0.0;
	arrayCells[posS].h = 0.0;
	arrayCells[posS].parentRow = start[0];
	arrayCells[posS].parentCol = start[1];
}

/* This function returns the Euclidean distance between two cells. */
double distance (Cell a, Cell b) {
	return ((double)sqrt((a.row - b.row)*(a.row - b.row) + (a.col - b.col)*(a.col - b.col)));
}

/* Check if a neighbor is valid (i.e., if it is inside the map) and free. If it is, it is returned true, otherwise false. */
bool check_a_neighbor(int deltaRow, int deltaCol, int cell[], bool map[]){
	int neighbor[2];
	neighbor[0] = cell[0] + deltaRow;
	neighbor[1] = cell[1] + deltaCol;
	if((check_position(neighbor)) && (is_free(neighbor, map)))
		return true;
	else
		return false;
}

/* Evaluation of the best parent (in terms of g) for the cell thisCell[]. */
int chooseBestParent(Cell arrayCells[], bool map[], int thisCell[], int bestParent[]){
	int thisCellPos = calculatePos(thisCell);
	int deltaRow, deltaCol;
	int bpPos = calculatePos(bestParent);					
	int currentParent[2];
	int cpPos;
	for (deltaRow=-1; deltaRow<=1; deltaRow++){
		for (deltaCol=-1; deltaCol<=1; deltaCol++){
			if (deltaRow != 0 || deltaCol != 0){	// This excludes the cell itself from the evaluation	
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
	return bpPos;
}

/* Print the best path found. */
void printPath(Cell arrayCells[], int bestPath[], int bestPathSize, bool map[]){
	printf("\nGoal reached through %d intermedium cells. Path length %f over minimum distance %f (+ %.2f \%%).\n\n", bestPathSize-2, arrayCells[bestPath[0]].f, distance(arrayCells[bestPath[bestPathSize-1]], arrayCells[bestPath[0]]), ((arrayCells[bestPath[0]].f / distance(arrayCells[bestPath[bestPathSize-1]], arrayCells[bestPath[0]])) - 1)*100);
	int ripet = DIM / ARR_MAX;
	int remain = DIM % ARR_MAX;
	int r,c,n;
	char *row = malloc(2*ARR_MAX*sizeof(bool));
	
	FILE *fp;
	
	char fname[40];
	sprintf(fname,"./paths/path-dim%d-obst%d",DIM,OBSTACLES);
	
	fp = fopen(fname,"w");
	if (fp == NULL){
		printf("Error while opening the output file.\n");
		exit(1);
	} else {
		printf("Printing the path on the file... ");
		
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
								row[2*c] = 'G';		// Goal point
							} else if(arrayCells[posCell].row == arrayCells[bestPath[bestPathSize-1]].row && arrayCells[posCell].col == arrayCells[bestPath[bestPathSize-1]].col){
								row[2*c] = 'S';		// Starting point
							} else row[2*c] = 'O';	// Point of the path
						} else
							row[2*c] = '.';			// Free
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
							row[2*c] = 'G';		// Goal point
						} else if(arrayCells[posCell].row == arrayCells[bestPath[bestPathSize-1]].row && arrayCells[posCell].col == arrayCells[bestPath[bestPathSize-1]].col){
							row[2*c] = 'S';		// Starting point
						} else row[2*c] = 'O';	// Point of the path
					} else
						row[2*c] = '.';			// Free
				}
				row[2*c+1] = ' ';
			}
			row[2*remain] = '\0';
			fprintf(fp, "%s\n", row);
		}
		
		printf("Path correctly printed on the file. ");
		fclose(fp);
		printf("File correctly closed.\n\n");
	}
	free(row);
}

/* This function explicits the result of the search, i.e., if a path is found, then it is printed, otherwise a message
is printed that warns of the unreachability of the goal point. */ 
void endSearch(bool foundPath, Cell arrayCells[], int bestPath[], int bestPathSize, bool map[], int posStart, int posGoal) {
	if(foundPath) {
		printPath(arrayCells, bestPath, bestPathSize, map);
	} else {
		printf("\nGoal not reachable!\n");
	}
}

/* Free all dynamic vectors. */
void freeAll(int openSet[], int closedSet[], int path[], int bestPath[]){
	free(openSet);
	free(closedSet);
	free(path);
	free(bestPath);
}

/* Main part of the algorithm - here all operations are carried out to find the best path. */
void search (bool map[], int start[], int goal[]) {	
	printf("Filling arrayCells... ");
	Cell *arrayCells;
	arrayCells = (Cell *)malloc(DIM*DIM*sizeof(Cell));

	// Check if the allocation is done correctly 
	if(arrayCells == NULL) {
		printf("\nFailed allocation.\n");
		return;
	}

	// Initialization of each cell
	initCells(arrayCells, start, goal);
	
	int posS = calculatePos(start);
	int posG = calculatePos(goal);

	// Initialization of 4 vectors, i.e., openSet, closedSet, path, bestPath
	
	int *openSet;			// Open set (priority queue) - it contains all the cells that step by step will have to be considered 
	int allocOpen = ALLOC;
	openSet = (int*)malloc(sizeof(int)*allocOpen);
	openSet[0] = posS;		// The first cell in the open set is the starting cell
	int openSetSize = 1;

	int *closedSet;			// Closed set - it contains all the cells already considered 
	int allocClosed = ALLOC;
	closedSet = (int*)malloc(sizeof(int)*allocClosed);
	int closedSetSize = 0;
	
	bool foundPath = false;
	
	int *path;				// Path between the starting point and the goal point
	int allocPath = ALLOC;
	path = (int*)malloc(sizeof(int)*allocPath);
	int pathSize = 0;
	
	int *bestPath;
	bestPath = (int*)malloc(sizeof(int)*allocPath);
	int bestPathSize = 0;

	printf("Searching the best path...\n");
	
//	double somma_time = 0, tot_time = 0, s1 = 0, e1 = 0, t1 = 0, s2 = 0, e2 = 0, t2 = 0;
//	double start_time, st, et, end_time;

	while (1) {
		int c[2];		// "c" is the current cell
		bool isThereBest = false;
		int best = 0;	// Initial assumption: the cell having the lowest value of "f" is in the first position of the "openSet"
		
		// Scan the "openSet" to find the new best cell		
		for (int i = 0; i < openSetSize; i++) {
		    double tmpF = arrayCells[openSet[i]].f;
	    	if (tmpF <= arrayCells[openSet[best]].f) {
				if ((foundPath && tmpF <= arrayCells[bestPath[0]].f) || !foundPath){
					isThereBest = true;
					best = i;
				}
			}
		}

		// Condition that terminates the algorithm 		
		if((foundPath && !isThereBest) || openSetSize == 0){
			endSearch(foundPath, arrayCells, bestPath, bestPathSize, map, posS, posG);
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
				
		// Remove the current cell from the "openSet"
		for (int i = best; i < openSetSize; i++) {
			openSet[i] = openSet[i+1];
		}
		openSetSize--;
		
		// Add the current cell inside the closed set + possible reallocation of "closedSet" vector
		if(closedSetSize >= allocClosed){
			allocClosed += ALLOC;
			closedSet = (int*)realloc(closedSet, allocClosed*sizeof(int));
		}
		
		closedSet[closedSetSize] = posC;
		closedSetSize++;
				
		if (is_goal(c, goal)) {					// Check whether the current cell is the goal point or not
			int thisCell[2] = {c[0], c[1]};		// thisCell = c
			int thisCellPos = posC;
							
			path[0] = thisCellPos;				// Backward path (from goal to start)
			pathSize = 1;
			
			// These instructions are executed every time a cell has a parent (the loop stops when the starting cell is evaluated, whose parent is the cell itself) 
			while ((arrayCells[thisCellPos].parentRow != -1) && (arrayCells[thisCellPos].row != arrayCells[thisCellPos].parentRow || arrayCells[thisCellPos].col != arrayCells[thisCellPos].parentCol)) {
				
				// Possible reallocation of "path" and "bestPath" vectors 
				if(pathSize >= allocPath){
					allocPath += ALLOC;
					path = (int*)realloc(path, allocPath*sizeof(int));
					bestPath = (int*)realloc(bestPath, allocPath*sizeof(int));
				}
				
				// Choose the best parent for "thisCell"
				int bestParent[2] = {arrayCells[thisCellPos].parentRow, arrayCells[thisCellPos].parentCol};
				int bpPos = chooseBestParent(arrayCells, map, thisCell, bestParent);
				
				// "thisCell" is updated to the best parent
				thisCellPos = bpPos;
				bestParent[0] = arrayCells[bpPos].row;
				bestParent[1] = arrayCells[bpPos].col;
				thisCell[0] = bestParent[0];
				thisCell[1] = bestParent[1];
				
				path[pathSize] = bpPos;			// Add the parent cell to the path
				pathSize++;
			}
			
			// Updates "bestPath" if it is the first path found or if the new path is better than the previous one 
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
					
					// Possible reallocation of "closedSet" vector
					if(closedSetSize >= allocClosed){
						allocClosed += ALLOC;
						closedSet = (int*)realloc(closedSet, allocClosed*sizeof(int));
					}		

					// Add the cell to the "closedSet"
					closedSet[closedSetSize] = openSet[i];
					closedSetSize++;
					
					for (int j = i; j < openSetSize; j++){					
						// remove the cell from the OpenSet
						openSet[j] = openSet[j+1];
					}
					openSetSize--;
				}
			}
		}
		
		int numNeighbors = 0;
		int neighbor[2];			// Row and column of a neighbor
		int tmp[CONNECTIVITY];		// Vector that will contain all the possible neighbors of "c"
		
		// For cell "c", each possible neighbor is checked
		int deltaRow, deltaCol;
		for (deltaRow=-1; deltaRow<=1; deltaRow++){
			for (deltaCol=-1; deltaCol<=1; deltaCol++){
				if (deltaRow != 0 || deltaCol != 0){	// This excludes the cell itself from the evaluation
					if (check_a_neighbor(deltaRow, deltaCol, c, map)) {
						neighbor[0] = c[0] + deltaRow;
						neighbor[1] = c[1] + deltaCol;
						tmp[numNeighbors] = neighbor[0]*DIM + neighbor[1];
						numNeighbors++;
					}
				}
			}
		}
	
		int neighbors[numNeighbors];
		for (int i=0; i<numNeighbors; i++){
			neighbors[i] = tmp[i];
		}
				
		int i, j, k, posN;
		double tmpG;
		bool isInClosedSet, newOpenSetCell;
		
		// Loop for checking every neighbor of the current cell	
		for (i = 0; i < numNeighbors; i++) {
			neighbor[0] = arrayCells[neighbors[i]].row;
			neighbor[1] = arrayCells[neighbors[i]].col;
			posN = calculatePos(neighbor);
			isInClosedSet = false;
			
			// Check if the neighbor is already in the closed set. If it is, nothing is done, otherwise it is evaluated
			#pragma omp parallel for private(j) shared(isInClosedSet)
			for (j = 0; j < closedSetSize; j++) {
				if(!isInClosedSet){
					if (neighbor[0] == arrayCells[closedSet[j]].row && neighbor[1] == arrayCells[closedSet[j]].col) {
						isInClosedSet = true;
					}
				}
			}

			if(!isInClosedSet){
				tmpG = arrayCells[posC].g + distance(arrayCells[posC], arrayCells[posN]);					
				if(c[0] == start[0] && c[1] == start[1]){ 	// Check if I am at the beginning, i.e., if the cell is the starting point
					
					// Possible reallocation of "openSet" vector
					if(openSetSize >= allocOpen){
						allocOpen += ALLOC;
						openSet = (int*)realloc(openSet, allocOpen*sizeof(int));
					}
					
					// Add "neighbor" to the "openSet"
					openSet[openSetSize] = posN;
					arrayCells[posN].g = tmpG;
					openSetSize++;
					
				} else {		// General case, i.e., I am not at the begininng (the cell is NOT the starting point)

					newOpenSetCell = false;
					
					// Check if the neighbor is already in the open set. If it is NOT, a new cell was discovered				
					for (k = 0; k < openSetSize; k++) {
						if (neighbor[0] == arrayCells[openSet[k]].row && neighbor[1] == arrayCells[openSet[k]].col) {

								// If I am here, the neighbor is ALREADY in the open set

								// Check if the neighbor has been reached with a lower cost than before. If yes, its value of "g" is updated, otherwise nothing is done 
								if (tmpG < arrayCells[posN].g) {
									arrayCells[posN].g = tmpG;
								}
								k = openSetSize;  // £xit the "for" loop
						} else if (k == openSetSize - 1) {

							// If I am here, the neighbor is NOT in the "openSet", i.e., new cell discovered
							newOpenSetCell = true;
						}
					}

					if (newOpenSetCell || openSetSize == 0){
					
						// Possible reallocation of "openSet" vector
						if(openSetSize >= allocOpen){
							allocOpen += ALLOC;
							openSet = (int*)realloc(openSet, allocOpen*sizeof(int));
						}
						
						// Add "neighbor" to the "openSet"
						if(!foundPath || (foundPath && tmpG + distance(arrayCells[posN], arrayCells[posG]) < arrayCells[bestPath[0]].f)){
							arrayCells[posN].g = tmpG;
							openSet[openSetSize] = posN;
							openSetSize++;
						}
					}
				}

				arrayCells[posN].h = distance(arrayCells[posN], arrayCells[posG]);
				arrayCells[posN].f = arrayCells[posN].g + arrayCells[posN].h;
				arrayCells[posN].parentRow = c[0];
				arrayCells[posN].parentCol = c[1];			
			}
		}
	}
}

int main () {	
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
		// Execute the algorithm
		search(map, start, goal);
	}
	else {
		return 0;
	}
	return 1;
}
