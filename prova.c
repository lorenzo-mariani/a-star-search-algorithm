#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define DIM 225			// must be > 0
#define CONNECTIVITY 4	// it must be 4 or 8
#define ALLOC 25			// allocazione dinamica iniziale dei vettori
#define OBSTACLES 20	// obstacles percentage

bool map[DIM][DIM];			/* forse dichiarare map globalmente, e farne riferimento tramite puntatore? */

typedef struct {
	int row, col;	// Row and column of a cell
	double f;	// Total cost of the cell
	double g;	// Distance between the current cell and the starting point
	double h;	// Estimated distance between the current cell and the goal point
	int parentRow, parentCol;	// Row and column of the parent cell 
} Cell;

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
void fillMap(bool map[][DIM]){
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
bool is_free (int cell[], bool map[][DIM]) {
	if (map[cell[0]][cell[1]] == true)
		return true;
	else
		return false;
}

// Check the correctness of the start and goal cells
bool check (int start[], int goal[], bool map[][DIM]) {
	if (!(check_position(start) && check_position(goal))) {
		printf("Start or goal point is out of the map\n");
		return false;
	}

	if (!(is_free(start, map) && is_free(goal, map))) {
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
bool check_a_neighbor(int deltaRow, int deltaCol, int cell[], bool map[][DIM]){
	int neighbor[2];
	neighbor[0] = cell[0] + deltaRow;
	neighbor[1] = cell[1] + deltaCol;
	if((check_position(neighbor)) && (is_free(neighbor, map)))
		return true;
	else
		return false;
}

// Returns the position of the cell in the vector arrayCells[] used in the search() function
int calculatePos(int cell[]){
	return cell[0]*DIM + cell[1];
}

// Initialization of every cell
void initCells(Cell arrayCells[], int start[], int goal[]){	
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
void printPath(Cell arrayCells[], int bestPath[], int bestPathSize, bool map[][DIM]){
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
int chooseBestParent(Cell arrayCells[], bool map[][DIM], int thisCell[], int bestParent[]){
	int thisCellPos = calculatePos(thisCell);
	int deltaRow, deltaCol;
	int bpPos = calculatePos(bestParent);					
	int currentParent[2];
	int cpPos;
	for (deltaRow=-1; deltaRow<=1; deltaRow++){
		for (deltaCol=-1; deltaCol<=1; deltaCol++){
			if (deltaRow != 0 || deltaCol != 0){	// this excludes the cell itself
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

// Free all the dynamic vectors we used.
void freeAll(int openSet[], int closedSet[], int path[], int bestPath[]){
	free(openSet);
	free(closedSet);
	free(path);
	free(bestPath);
	printf("\nOk, memory is free.");
}

// Explicits the result of the search.
void endSearch(bool foundPath, Cell arrayCells[], int bestPath[], int bestPathSize, bool map[][DIM]) {
	if(foundPath) {
		printPath(arrayCells, bestPath, bestPathSize, map);
	} else {
		printf("\n\nThe goal is not reachable.\n");
	}
}

// Function to find the shortest path between the starting point and the goal point
void search (bool map[][DIM], int start[], int goal[]) {
	
	printf("AAAAAAAAAAAAAA");
	
	Cell arrayCells[DIM*DIM];	// Array containining the details of all cells

	// Initialization of each cell
	int posS = calculatePos(start);
	int posG = calculatePos(goal);
	initCells(arrayCells, start, goal);

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
}

int main () {

	if (!checkDefine()){
		return 0;
	}

	
	fillMap(map);
	
	int start[] = {3, 3};
	int goal[] = {DIM-3, DIM-3};
	
	if (check(start, goal, map)) {
		// Execute the algorithm
		search(map, start, goal);
	}
	else {
		return 0;
	}
	return 1;
}
