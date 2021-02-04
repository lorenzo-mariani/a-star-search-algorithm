#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define ROW 7 	// the map must be consistent wrt ROW, COL
#define COL 7
#define CONNECTIVITY 8	// it must be 4 or 8

typedef struct {
	int row, col;	// Row and column of a cell
	double f;	// Total cost of the cell
	double g;	// Distance between the current cell and the starting point
	double h;	// Estimated distance between the current cell and the goal point
	int parentRow, parentCol;	// Row and column of the parent cell 
} Cell;

// Check if the current cell is inside the map
bool check_position (int row, int col) {
	if (row >= 0 && row < ROW && col >= 0 && col < COL)
		return true;
	else
		return false; 
}

// Check if the goal point has been reached
bool is_goal (int row, int col, Cell goal) {
	if (row == goal.row && col == goal.col)
		return true;
	else
		return false;
}

// Check if the current cell is free or if there is an obstacle
bool is_free (int row, int col, bool map[][COL]) {
	if (map[row][col] == true)
		return true;
	else
		return false;
}

// Check the correctness of the start and goal cells
int check (Cell start, Cell goal, bool map[][COL]) {
	if ((!check_position(start.row, start.col)) || (!check_position(goal.row, goal.col))) {
		printf("Start/goal point out of the map\n");
		return 0;
	}

	if ((!is_free(start.row, start.col, map)) || (!is_free(goal.row, goal.col, map))) {
		printf("Start/goal point not reachable\n");
		return 0;
	}

	if (is_goal(start.row, start.col, goal)) {
		printf("Start and goal points are the same\n");
		return 0;
	}
	return 1;
}

// Computation of the heuristic
double heuristic (Cell a, Cell b) {
	
	double distance;

	if (CONNECTIVITY == 4) {
		distance = ((double)((abs(a.row - b.row))+(abs(a.col - b.col))));	// Manhattan distance
	}
	else if (CONNECTIVITY == 8) {
		distance = ((double)sqrt((a.row - b.row)*(a.row - b.row) + (a.col - b.col)*(a.col - b.col)));	// Euclidean distance
	}  
	
	return distance;
}

// Check if a neighbor is valid 
bool check_a_neighbor(int deltaRow, int deltaCol, Cell c, bool map[][COL]){
	if((check_position(c.row + deltaRow, c.col + deltaCol)) && (is_free(c.row + deltaRow, c.col + deltaCol, map)))
		return true;
	else
		return false;
}

// Function to find the shortest path between the starting point and the goal point
void search (bool map[][COL], Cell start, Cell goal) {

	Cell arrayCells[ROW*COL];	// Array containining the details of each cell
	
	// Initialization of each cell
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			int pos = i*ROW + j;
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
	int pos_start = start.row*ROW + start.col;
	arrayCells[pos_start].f = 0.0;
	arrayCells[pos_start].g = 0.0;
	arrayCells[pos_start].h = 0.0;
	arrayCells[pos_start].parentRow = start.row;
	arrayCells[pos_start].parentCol = start.col;

	Cell *openSet;	// Priority queue
	openSet = (Cell*)malloc(sizeof(Cell)*ROW*COL);
	openSet[0] = start;		// The first cell in the open set is the starting cell
	int openSetSize = 1;

	Cell *closedSet;	// Cells already visited
	closedSet = (Cell*)malloc(sizeof(Cell)*ROW*COL);
	int closedSetSize = 0;
	
	Cell *path;		// Path between the starting point and the goal point
	path = (Cell*)malloc(sizeof(Cell)*ROW*COL);

	while (openSetSize > 0) {
		
		for (int i = 0; i < openSetSize; i++) {
			printf("Cella open set: %d %d\n", openSet[i].row, openSet[i].col);
		}
		
		int best = 0;	// Initial assumption: the cell having the lowest value of f is in the first position of the open set

		// Scan the open set to find the new best cell
		for (int i = 0; i < openSetSize; i++) {
			if (openSet[i].f < openSet[best].f)
				best = i;
		}

		Cell c = openSet[best];		// c is the current cell
		printf("Cella corrent: %d %d\n", c.row, c.col);

		// If the current cell is the goal point, the algorithm stops
		if (is_goal(c.row, c.col, goal)) {
			
			Cell tmp = c;	// Temporary cell initialized to the current cell (goal point)
			int pathSize = 0;
			
			path[pathSize] = tmp;	// Adding the cell in the first position of the path
			pathSize++;
			
			// These instructions are executed every time a cell has a parent (the loop stops when the starting cell is evaluated, whose parent is the cell itself) 
			while ((tmp.parentRow != -1) && (tmp.row != tmp.parentRow && tmp.col != tmp.parentCol)) {
				int pos = tmp.parentRow*ROW + tmp.parentCol; // Variable used to find the position of the cell within arrayCells 
				
				// path = (Cell*)realloc(path, (pathSize+1)*sizeof(Cell));
				path[pathSize] = arrayCells[pos];	// Adding the parent cell to the path
				tmp = arrayCells[pos];	// Temporary cell value updated to the parent cell 
				pathSize++;
			}
			
			printf("Goal reached! The computed path is:\n");
			
			// Print the path
			for (int j = pathSize-1; j > 0; j--) {
				printf("(%d, %d)\n", path[j].row, path[j].col);
			}
			
			/* ATTENZIONE: QUELLO TROVATO POTREBBE NON ESSERE IL PERCORSO MIGLIORE! */

			free(openSet);
			free(closedSet);
			free(path);
			
			return;
		}
		
		// Removing the current cell from the open set
		for (int i = best; i < openSetSize; i++) {
			openSet[i] = openSet[i + 1];
		}
		// openSet = (Cell*)realloc(openSet, (openSetSize - 1)*sizeof(Cell));
		openSetSize--;

		// Adding the current cell inside the closed set
		// closedSet = (Cell*)realloc(closedSet, (closedSetSize + 1)*sizeof(Cell));
		closedSet[closedSetSize] = c;
		closedSetSize++;
		
		int numNeighbors = 0;
		Cell neighbor;
		Cell tmp[CONNECTIVITY];
		
		// For cell "c", each possible neighbor (from high-left to low-right) is checked.
		int deltaRow, deltaCol;
		for (deltaRow=-1; deltaRow<=1; deltaRow++){
			for (deltaCol=-1; deltaCol<=1; deltaCol++){
				if (deltaRow != 0 || deltaCol != 0){	// this excludes the cell itself
					if (CONNECTIVITY == 8 || (deltaRow == 0 || deltaCol == 0)){		// this check the connectivity and works consequently
						if (check_a_neighbor(deltaRow, deltaCol, c, map)) {
							neighbor.row = c.row + deltaRow;
							neighbor.col = c.col + deltaCol;
							tmp[numNeighbors] = neighbor;
							numNeighbors++;
						}
					}
				}
			}
		}
	
		Cell neighbors[numNeighbors];
		for (int i=0; i<numNeighbors; i++){
			neighbors[i] = tmp[i];
			printf(" Neighbor found: %d %d,\n", neighbors[i].row, neighbors[i].col);
		}
		
		//int neighborSize = sizeof(neighbors)/sizeof(neighbors[0]);
		//printf("\nTot: %d / %d = %d neighbors\n", sizeof(neighbors), sizeof(neighbors[0]), neighborSize);
		
		// Loop for checking every neighbor of the current cell
		for (int i = 0; i < numNeighbors; i++) {
			neighbor = neighbors[i];
			printf(" Neighbor: %d\%d\n", neighbor.row, neighbor.col);
			
			// Check if the neighbor is already in the closed set. If the negighbor is already
			// in the closed set it is evaluated, otherwise nothing is done
			for (int j = 0; j < closedSetSize; j++) {
				if (neighbor.row != closedSet[j].row || neighbor.col != closedSet[j].col) {
					if (j == closedSetSize - 1) {
						
						// If I am here, the neighbor is NOT in the closed set
						
						double tmpG = c.g + heuristic(c, neighbor);
						printf("distance: %f. ClosedsetSize: %d. OpensetSize: %d\n", tmpG, closedSetSize, openSetSize);
						
						// If I am at the beginning (the cell is the start)
						if(c.row == start.row && c.col == start.col){

							arrayCells[neighbor.row*ROW + neighbor.col].g = tmpG;

							// Inserisco "neighbor" all'interno dell'open set
							//openSet = (Cell*)realloc(openSet, (openSetSize+1)*sizeof(Cell));
							openSet[openSetSize] = arrayCells[neighbor.row*ROW + neighbor.col];
							openSet[openSetSize].h = heuristic(neighbor, goal);
							openSet[openSetSize].f = openSet[openSetSize].g + openSet[openSetSize].h;
							openSetSize++;
							printf("\nNow opensetSize=%d\n", openSetSize);
						}

						// Check if the neighbor is already in the open set. If it is NOT, it means that a new cell was discovered
						else {
							for (int k = 0; k < openSetSize; k++) {
								if (neighbor.row == openSet[k].row && neighbor.col == openSet[k].col) {

										// If I am here, the neighbor is already in the open set

										// Check if the neighbor has been reached with a lower cost than before. 
										// If yes, its value of g is updated, otherwise nothing is done 
										if (tmpG < arrayCells[neighbor.row*ROW + neighbor.col].g) {
											arrayCells[neighbor.row*ROW + neighbor.col].g = tmpG;
											openSet[k].g = tmpG;
											//neighbor.g = tmpG;
											openSet[k].h = heuristic(neighbor, goal);
											openSet[k].f = openSet[k].g + openSet[k].h;
										}
											
								}
								else if (k == openSetSize - 1) {
									// If I am here, the neighbor is NOT in the open set

									arrayCells[neighbor.row*ROW + neighbor.col].g = tmpG;
									neighbor.g = tmpG;

									// Inserisco "neighbor" all'interno dell'open set
									//openSet = (Cell*)realloc(openSet, (openSetSize+1)*sizeof(Cell));
									openSet[openSetSize] = neighbor;
									//openSet[openSetSize] = arrayCells[neighbor.row*ROW + neighbor.col];
									openSet[openSetSize].h = heuristic(neighbor, goal);
									openSet[openSetSize].f = openSet[openSetSize].g + openSet[openSetSize].h;
									openSetSize++;
									printf("\nNow opensetSize=%d. closedsetSize=%d\n", openSetSize, closedSetSize);
								}
							}
						}

						arrayCells[neighbor.row*ROW + neighbor.col].h = heuristic(neighbor, goal);
						arrayCells[neighbor.row*ROW + neighbor.col].f = arrayCells[neighbor.row*ROW + neighbor.col].g + arrayCells[neighbor.row*ROW + neighbor.col].h;
						printf("\nf: %f\n", arrayCells[neighbor.row*ROW + neighbor.col].f);				
						arrayCells[neighbor.row*ROW + neighbor.col].parentRow = c.row;
						arrayCells[neighbor.row*ROW + neighbor.col].parentCol = c.col;
					}
				}
			}
		}
	}
	
	free(openSet);
	free(closedSet);

	return;
}

int main () {
	
	// false - There is an obstacle
	// true - The cell is free
	bool map[ROW][COL] = 
	{
		{true, true, true, true, true, true, true},
		{true, true, false, false, true, true, true},
		{true, true, false, false, true, true, true},
		{true, false, false, false, true, true, true},
		{true, true, false, false, true, true, true},
		{true, true, true, true, false, false, true},
		{true, false, true, true, true, true, true}
	};

	Cell start;	// Starting point
	start.row = 6; 
	start.col = 2;
	
	Cell goal;	// Goal point
	goal.row = 1;
	goal.col = 6;

	if (check(start, goal, map) == 1) {
		// Execute the algorithm
		search(map, start, goal);
	}
	else {
		return 0;
	}

	return 1;
}
