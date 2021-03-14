#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define DIM 500			// must be > 0
#define CONNECTIVITY 4	// it must be 4 or 8
#define ALLOC 25			// allocazione dinamica iniziale dei vettori
#define OBSTACLES 20	// obstacles percentage

bool map[DIM][DIM];

typedef struct {
	int row, col;	// Row and column of a cell
	double f;	// Total cost of the cell
	double g;	// Distance between the current cell and the starting point
	double h;	// Estimated distance between the current cell and the goal point
	int parentRow, parentCol;	// Row and column of the parent cell 
} Cell;

Cell arrayCells[DIM*DIM];	// Array containining the details of all cells

// check the values defined at the beginning
bool checkDefine(){
	if (DIM <= 0){
		printf("\n%d is not a valid dimension.\n", DIM);
		return false;
	} else if (CONNECTIVITY != 4 && CONNECTIVITY != 8){
		printf("\n%d is not a valid connectivity.\n", CONNECTIVITY);
		return false;
	} else if (ALLOC <= 0){
		printf("\n%d is not a valid value for reallocation.\n", ALLOC);
		return false;
	} else if (OBSTACLES < 0 || OBSTACLES > 100){
		printf("\n%d is not a valid obstacle percentage.\n", OBSTACLES);
		return false;
	} else {
		return true;
	}
}

// fills the map at the beginning, with random values
void fillMap(){
	int elem;
	int obstPercent = OBSTACLES;		// to avoid division by 0
	if(obstPercent != 0){
		int obst = 100/obstPercent;	// 1 over obst is an obstacle
		srand(time(NULL));	// CASUAL
		//srand(0);				// NOT CASUAL
		printf("\n\n");
		for (int r=0; r<DIM; r++){
			for (int c=0; c<DIM; c++){
				elem = rand()%obst;		// "elem" is a pseudo-random integer in [0; obst]
				if (elem == 0){
					map[r][c] = false;		// not free
					//printf("X ");
				} else {
					map[r][c] = true;		// free
					//printf(". ");
				}
			}
			//printf("\n");
		}
	} else {
		printf("\n\n");
		for (int r=0; r<DIM; r++){
			for (int c=0; c<DIM; c++){
				map[r][c] = true;		// free
				printf(". ");
			}
			printf("\n");
		}
	}
	printf("\n\n");
}

// Check if the current cell is inside the map
bool check_position (int cell[]) {
	if (cell[0] >= 0 && cell[0] < DIM && cell[1] >= 0 && cell[1] < DIM)
		return true;
	else
		return false; 
}

// Check if the goal point has been reached
bool is_goal (int start[], int goal[]) {
	if (start[0] == goal[0] && start[1] == goal[1])
		return true;
	else
		return false;
}

// Check if the current cell is free or if there is an obstacle
bool is_free (int cell[]) {
	if (map[cell[0]][cell[1]] == true)
		return true;
	else
		return false;
}

// Check the correctness of the start and goal cells
bool check (int start[], int goal[]) {
	if (!(check_position(start) && check_position(goal))) {
		printf("Start or goal point is out of the map\n");
		return false;
	}

	if (!(is_free(start) && is_free(goal))) {
		printf("Start or goal point is not free\n");
		return false;
	}

	if (is_goal(start, goal)) {
		printf("Start and goal points coincide\n");
		return false;
	}
	return true;
}

// Computate distance between two cells
double heuristic (Cell a, Cell b) {
	
	double distance;

	if (CONNECTIVITY == 4) {
		distance = ((double)((abs(a.row - b.row))+(abs(a.col - b.col))));	// Manhattan distance
	}
	else {
		distance = ((double)sqrt((a.row - b.row)*(a.row - b.row) + (a.col - b.col)*(a.col - b.col)));	// Euclidean distance
	}  
	return distance;
}

// Check if a neighbor is valid and free 
bool check_a_neighbor(int deltaRow, int deltaCol, int cell[]){
	int neighbor[2];
	neighbor[0] = cell[0] + deltaRow;
	neighbor[1] = cell[1] + deltaCol;
	if((check_position(neighbor)) && (is_free(neighbor)))
		return true;
	else
		return false;
}

// Returns the position of the cell in the vector arrayCells[] used in the search() function
int calculatePos(int cell[]){
	return cell[0]*DIM + cell[1];
}

// Initialization of every cell
void initCells(int start[], int goal[]){	
	for (int i = 0; i < DIM; i++) {
		for (int j = 0; j < DIM; j++) {
			int pos = i*DIM + j;
			arrayCells[pos].row = i;
			arrayCells[pos].col = j;
			arrayCells[pos].f = 100000.0;	// very high number
			arrayCells[pos].g = 100000.0;	// very high number
			arrayCells[pos].h = 100000.0;	// very high number
			arrayCells[pos].parentRow = -1;	// invalid position
			arrayCells[pos].parentCol = -1;	// invalid position
		}
	}
	
	// Initialization of the starting cell
	int posS = calculatePos(start);
	arrayCells[posS].f = 0.0;
	arrayCells[posS].g = 0.0;
	arrayCells[posS].h = 0.0;
	arrayCells[posS].parentRow = start[0];
	arrayCells[posS].parentCol = start[1];
}

// At the end of the analysis, prints the best path found
void printPath(int bestPath[], int bestPathSize){
	printf("\nGoal reached through %d cells. Path length %f over minimum distance %f (+ %.2f \%%).\n\nThe computed path is: ", bestPathSize, arrayCells[bestPath[0]].f, heuristic(arrayCells[bestPath[bestPathSize-1]], arrayCells[bestPath[0]]), ((arrayCells[bestPath[0]].f / heuristic(arrayCells[bestPath[bestPathSize-1]], arrayCells[bestPath[0]])) - 1)*100);
	// Print the list of cells in the path
	for (int j = bestPathSize-1; j >= 0; j--) {
		printf("(%d, %d) ", arrayCells[bestPath[j]].row, arrayCells[bestPath[j]].col);
	}
	// Draw the map
	printf("\n\n\n");	
	for (int r=0; r<DIM; r++){
		for (int c=0; c<DIM; c++){
			if (!map[r][c]){
				printf("- ");
			} else {
				bool ispath = false;
				for (int b=0; b<bestPathSize; b++){
					int cellPos = r*DIM + c;
					if(arrayCells[cellPos].row == arrayCells[bestPath[b]].row && arrayCells[cellPos].col == arrayCells[bestPath[b]].col)
						ispath = true;
				}
				if(ispath) printf("O ");
				else printf("  ");
			}
		}
		printf("\n");
	}
	printf("\n\n");
}

// Evaluates the best parent (in terms of g) for the cell thisCell[].
int chooseBestParent( int thisCell[], int bestParent[]){
	int thisCellPos = calculatePos(thisCell);
	int deltaRow, deltaCol;
	int bpPos = calculatePos(bestParent);					
	int currentParent[2];
	int cpPos;
	for (deltaRow=-1; deltaRow<=1; deltaRow++){
		for (deltaCol=-1; deltaCol<=1; deltaCol++){
			if (deltaRow != 0 || deltaCol != 0){	// this excludes the cell itself
				if (CONNECTIVITY == 8 || (deltaRow == 0 || deltaCol == 0)){		// this checks the connectivity and works consequently
					if (check_a_neighbor(deltaRow, deltaCol, thisCell)) {
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

// Free all the dynamic vectors we used.
void freeAll(int openSet[], int closedSet[], int path[], int bestPath[]){
	free(openSet);
	free(closedSet);
	free(path);
	free(bestPath);
	printf("\nOk, memory is free.");
}

// Explicits the result of the search.
void endSearch(bool foundPath, int bestPath[], int bestPathSize) {
	if(foundPath) {
		printPath(bestPath, bestPathSize);
	} else {
		printf("\n\nThe goal is not reachable.\n");
	}
}

// Function to find the shortest path between the starting point and the goal point
void search (int start[], int goal[]) {	
	

	// Initialization of each cell
	int posS = calculatePos(start);
	int posG = calculatePos(goal);
	initCells(start, goal);

	// Initialization of 4 vectors
	
	int *openSet;	// Priority queue
	int allocOpen = ALLOC;
	openSet = (int*)malloc(sizeof(int)*allocOpen);
	openSet[0] = posS;		// The first cell in the open set is the starting cell
	int openSetSize = 1;

	int *closedSet;	// Cells already visited
	int allocClosed = ALLOC;
	closedSet = (int*)malloc(sizeof(int)*allocClosed);
	int closedSetSize = 0;
	
	bool foundPath = false;
	
	int *path;		// Path between the starting point and the goal point
	int allocPath = ALLOC;
	path = (int*)malloc(sizeof(int)*allocPath);
	int pathSize = 0;
	
	int *bestPath;
	bestPath = (int*)malloc(sizeof(int)*allocPath);
	int bestPathSize = 0;

	while (1) {
		//printf("\nNum celle Open Set: %d\n", openSetSize);
		/*for (int i = 0; i < openSetSize; i++) {
			printf("\nCella open set: %d %d with f=%f, g=%f, h=%f", arrayCells[openSet[i]].row, arrayCells[openSet[i]].col, arrayCells[openSet[i]].f, arrayCells[openSet[i]].g, arrayCells[openSet[i]].h);
		}*/
		
		int c[2];		// c is the current cell
		bool isThereBest = false;
		int best = 0;	// Initial assumption: the cell having the lowest value of f is in the first position of the open set
		bool isNew;
		
		// Scan the open set to find the new best cell
		for (int i = 0; i < openSetSize; i++) {
			isNew = true;
			//if (arrayCells[openSet[i]].f <= arrayCells[openSet[best]].f && arrayCells[openSet[i]].h < arrayCells[openSet[best]].h) {		// SICURI CHE SERVA CONSIDERARE H ???
			if (arrayCells[openSet[i]].f <= arrayCells[openSet[best]].f) {
				if ((foundPath && arrayCells[openSet[i]].f <= arrayCells[bestPath[0]].f) || !foundPath){
					/*for (int j = 0; j < closedSetSize; j++){
						if (arrayCells[openSet[i]].row == arrayCells[closedSet[j]].row && arrayCells[openSet[i]].col == arrayCells[closedSet[j]].col){		// MA A COSA SERVE ???
							isNew = false;
							j = closedSetSize;		// exit the loop
						}
					}
					if (isNew) {
						best = i;*/
						isThereBest = true;
					/*}*/
				}
			}
		}
				
		if((foundPath && !isThereBest) || openSetSize == 0){
			// final functions are called
			endSearch(foundPath, bestPath, bestPathSize);
			freeAll(openSet, closedSet, path, bestPath);
			return;
		}
		
		c[0] = arrayCells[openSet[best]].row;
		c[1] = arrayCells[openSet[best]].col;
		
		int posC = calculatePos(c);
		
		//printf("\n>>> Cella corrente: (%d %d)", arrayCells[posC].row, arrayCells[posC].col);	// 	c[0], c[1]
		
		if(posC == posS){
			arrayCells[posS].h = heuristic(arrayCells[posS], arrayCells[posG]);
			arrayCells[posS].f = arrayCells[posS].h;
		}
		
		if (is_goal(c, goal)) {		// if the considered cell corresponds to the goal
			int thisCell[2] = {c[0], c[1]};		// thisCell = c
			int thisCellPos = posC;
							
			path[0] = thisCellPos;		// path is backward (from goal to start)
			pathSize = 1;
			
			// These instructions are executed every time a cell has a parent (the loop stops when the starting cell is evaluated, whose parent is the cell itself) 
			while ((arrayCells[thisCellPos].parentRow != -1) && (arrayCells[thisCellPos].row != arrayCells[thisCellPos].parentRow || arrayCells[thisCellPos].col != arrayCells[thisCellPos].parentCol)) {
				
				// eventuale riallocazione vettori Path e BestPath
				if(pathSize >= allocPath){
					allocPath += ALLOC;
					path = (int*)realloc(path, allocPath*sizeof(int));
					bestPath = (int*)realloc(bestPath, allocPath*sizeof(int));
				}
				
				// choose the best parent for thisCell
				int bestParent[2] = {arrayCells[thisCellPos].parentRow, arrayCells[thisCellPos].parentCol};
				int bpPos = chooseBestParent(thisCell, bestParent);
				
				// thisCell is updated to the best parent
				thisCellPos = bpPos;
				bestParent[0] = arrayCells[bpPos].row;
				bestParent[1] = arrayCells[bpPos].col;
				thisCell[0] = bestParent[0];
				thisCell[1] = bestParent[1];
				
				path[pathSize] = bpPos;	// Adding the parent cell to the path
				pathSize++;
				//printf("\nPathsize: %d, aggiunta: %d %d con f= %f\n", pathSize, arrayCells[thisCellPos].row, arrayCells[thisCellPos].col, arrayCells[thisCellPos].f);
			}
			
			// update the BestPath if it is the first path I've found OR if the new path is better than the previous one.
			if(!foundPath || (foundPath && arrayCells[path[0]].f < arrayCells[bestPath[0]].f)) {
				for(int i = 0; i < pathSize; i++){
					bestPath[i] = path[i];
				}
				bestPathSize = pathSize;
			}
			
			foundPath = true;
			printf("\n\nPATH DISCOVERED\n\n");

			// 
			float pathF = arrayCells[path[0]].f;
			for(int i = 0; i < openSetSize; i++){
				if (arrayCells[openSet[i]].f >= pathF){
					for (int j = i; j < openSetSize; j++){
						
						// eventuale riallocazione vettore ClosedSet
						if(closedSetSize >= allocClosed){
							allocClosed += ALLOC;
							closedSet = (int*)realloc(closedSet, allocClosed*sizeof(int));
						}		
						// aggiungo la cella al ClosedSet
						closedSet[closedSetSize] = openSet[j];
						closedSetSize++;
						
						// rimuovo la cella da OpenSet
						openSet[j] = openSet[j+1];
						openSetSize--;
					}
				}
			}
		}
				
		// Removing the current cell from the open set
		for (int i = best; i < openSetSize; i++) {
			openSet[i] = openSet[i + 1];
		}
		openSetSize--;

		// Adding the current cell inside the closed set
		// Eventuale riallocazione vettore ClosedSet
		if(closedSetSize >= allocClosed){
			allocClosed += ALLOC;
			closedSet = (int*)realloc(closedSet, allocClosed*sizeof(int));
		}
		closedSet[closedSetSize] = posC;
		closedSetSize++;
		
		int numNeighbors = 0;
		int neighbor[2];			// row and column of a neighbor
		int tmp[CONNECTIVITY];		// it goes to contain all the possible neighbors of c
		
		// For cell "c", each possible neighbor is checked.
		int deltaRow, deltaCol;
		for (deltaRow=-1; deltaRow<=1; deltaRow++){
			for (deltaCol=-1; deltaCol<=1; deltaCol++){
				if (deltaRow != 0 || deltaCol != 0){	// this excludes the cell itself
					if (CONNECTIVITY == 8 || (deltaRow == 0 || deltaCol == 0)){		// this check the connectivity and works consequently
						if (check_a_neighbor(deltaRow, deltaCol, c)) {
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
		
		bool aNewNeighbor = false;		// serve solo nel pezzo di codice finale, che � commentato!!
		
		// Loop for checking every neighbor of the current cell
		for (int i = 0; i < numNeighbors; i++) {
			neighbor[0] = arrayCells[neighbors[i]].row;
			neighbor[1] = arrayCells[neighbors[i]].col;
			int posN = calculatePos(neighbor);
			//printf("   Neighbor: %d %d ", neighbor[0], neighbor[1]);
			
			// Check if the neighbor is already in the closed set.
			// If the neighbor is NOT in the closed set, it is evaluated, otherwise nothing is done.
			for (int j = 0; j < closedSetSize; j++) {
				if ((neighbor[0] != arrayCells[closedSet[j]].row || neighbor[1] != arrayCells[closedSet[j]].col) && (j == closedSetSize - 1)) {
					
					// If I am here, the neighbor is NOT in the closed set
					
					aNewNeighbor = true;		// serve solo nel pezzo di codice finale, che � commentato!!
					
					double tmpG = arrayCells[posC].g + heuristic(arrayCells[posC], arrayCells[posN]);
					//printf("distance: %f. ClosedsetSize: %d. OpensetSize: %d\n", tmpG, closedSetSize, openSetSize);
					
					if(c[0] == start[0] && c[1] == start[1]){ 	// If I am at the beginning (the cell is the start)

						// Eventuale riallocazione vettore OpenSet
						if(openSetSize >= allocOpen){
							allocOpen += ALLOC;
							openSet = (int*)realloc(openSet, allocOpen*sizeof(int));
						}
						// Inserisco "neighbor" all'interno dell'open set
						openSet[openSetSize] = posN;
						arrayCells[posN].g = tmpG;
						/*arrayCells[posN].h = heuristic(arrayCells[posN], arrayCells[posG]);
						arrayCells[posN].f = arrayCells[posN].g + arrayCells[posN].h;*/
						openSetSize++;
						//printf(", ADDED\n");
					} else {		// general case (not the cell start)
						bool newOpenSetCell = false;
						// Check if the neighbor is already in the open set. If it is NOT, it means that a new cell was discovered
						for (int k = 0; k < openSetSize; k++) {
							if (neighbor[0] == arrayCells[openSet[k]].row && neighbor[1] == arrayCells[openSet[k]].col) {
									// If I am here, the neighbor is ALREADY in the open set

									// Check if the neighbor has been reached with a lower cost than before. 
									// If yes, its value of g is updated, otherwise nothing is done 
									if (tmpG < arrayCells[posN].g) {
										arrayCells[posN].g = tmpG;
										/*arrayCells[posN].h = heuristic(arrayCells[posN], arrayCells[posG]);
										arrayCells[posN].f = arrayCells[posN].g + arrayCells[posN].h;*/
										/*arrayCells[posN].parentRow = c[0];
										arrayCells[posN].parentCol = c[1];*/
									}
									k = openSetSize;  // esce dal FOR
							} else if (k == openSetSize - 1) {
								// If I am here, the neighbor is NOT in the open set: new cell discovered
								newOpenSetCell = true;
							}
						}
						
						if (newOpenSetCell || openSetSize == 0){
							
							// Eventuale riallocazione vettore OpenSet
							if(openSetSize >= allocOpen){
								allocOpen += ALLOC;
								openSet = (int*)realloc(openSet, allocOpen*sizeof(int));
							}
							
							// Inserisco "neighbor" all'interno dell'open set
							arrayCells[posN].g = tmpG;
							openSet[openSetSize] = posN;
							/*arrayCells[posN].h = heuristic(arrayCells[posN], arrayCells[posG]);
							arrayCells[posN].f = arrayCells[posN].g + arrayCells[posN].h;*/
							//printf(", ADDED\n");
							openSetSize++;
						}
					}

					arrayCells[posN].h = heuristic(arrayCells[posN], arrayCells[posG]);
					arrayCells[posN].f = arrayCells[posN].g + arrayCells[posN].h;
					arrayCells[posN].parentRow = c[0];
					arrayCells[posN].parentCol = c[1];
				}
				else if (neighbor[0] == arrayCells[closedSet[j]].row && neighbor[1] == arrayCells[closedSet[j]].col) {
					j = closedSetSize;		// per uscire dal ciclo e cominciare con un nuovo neighbor
				}
			}
		}
		/*if(!aNewNeighbor && openSetSize == 0){		// SEMBRA NON SERVIRE: C'E' UN BLOCCO UGUALE A 275
			// the search is over!
			printf("\nSecond case\n");
			endSearch(foundPath, arrayCells, bestPath, bestPathSize, map);
			freeAll(openSet, closedSet, path, bestPath);
			return;
		}*/
	}
}

int main () {

	if (!checkDefine()){
		return 0;
	}

	fillMap();
	
	int start[] = {3, 3};
	int goal[] = {DIM-3, DIM-3};
	
	if (check(start, goal)) {
		// Execute the algorithm
		search(start, goal);
	}
	else {
		return 0;
	}
	return 1;
}
