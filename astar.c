#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define ROW 7 	// the map must be consistent wrt ROW, COL
#define COL 7
#define CONNECTIVITY 8	// it must be 4 or 8

typedef struct {
	int row, col;	// Row and column of a cell
	int numNeighbors;
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

// Search for cells adjacent to the current one
/*Cell * find_neighbors (bool map[][COL], Cell c, int *numNeighbors) {

	int counter = 0;
	Cell neighbor;
	Cell tmp[CONNECTIVITY];
	
	// Neighbor in position (row - 1, col)
	if ((check_position(c.row - 1, c.col)) && (is_free(c.row - 1, c.col, map))) {
		neighbor.row = c.row - 1;
		neighbor.col = c.col;
		tmp[counter] = neighbor;
		counter++;
	}

	// Neighbor in position (row, col + 1)
	if ((check_position(c.row, c.col + 1)) && (is_free(c.row, c.col + 1, map))) {
		neighbor.row = c.row;
		neighbor.col = c.col + 1;
		tmp[counter] = neighbor;
		counter++;
	}

	// Neighbor in position (row + 1, col)
	if ((check_position(c.row + 1, c.col)) && (is_free(c.row + 1, c.col, map))) {
		neighbor.row = c.row + 1;
		neighbor.col = c.col;
		tmp[counter] = neighbor;
		counter++;
	}

	// Neighbor in position (row, col - 1)
	if ((check_position(c.row, c.col - 1)) && (is_free(c.row, c.col - 1, map))) {
		neighbor.row = c.row;
		neighbor.col = c.col - 1;
		tmp[counter] = neighbor;
		counter++;
	}

	if (CONNECTIVITY == 8) {

		// Neighbor in position (row - 1, col + 1)
		if ((check_position(c.row - 1, c.col + 1)) && (is_free(c.row - 1, c.col + 1, map))) {
			neighbor.row = c.row - 1;
			neighbor.col = c.col + 1;
			tmp[counter] = neighbor;
			counter++;
		}

		// Neighbor in position (row + 1, col + 1)
		if ((check_position(c.row + 1, c.col + 1)) && (is_free(c.row + 1, c.col + 1, map))) {
			neighbor.row = c.row + 1;
			neighbor.col = c.col + 1;
			tmp[counter] = neighbor;
			counter++;
		}

		// Neighbor in position (row + 1, col - 1)
		if ((check_position(c.row + 1, c.col - 1)) && (is_free(c.row + 1, c.col - 1, map))) {
			neighbor.row = c.row + 1;
			neighbor.col = c.col - 1;
			tmp[counter] = neighbor;
			counter++;
		}

		// Neighbor in position (row - 1, col - 1)
		if (check_position(c.row - 1, c.col -  1) && (is_free(c.row - 1, c.col - 1, map))) {
			neighbor.row = c.row - 1;
			neighbor.col = c.col - 1;
			tmp[counter] = neighbor;
			counter++;	
		}
	}

	Cell neighbors[counter];
	for (int i=0; i<counter; i++){
		neighbors[i] = tmp[i];
		printf("\nfind: ");
		printf("%d %d, ", neighbors[i].row, neighbors[i].col);
	}
	*numNeighbors = counter;
	printf("prova");
	//printf("find: size: %d", sizeof(neighbors));
	printf("find: %d neighbors", *numNeighbors);
	return neighbors;
}*/

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

// Function to find the shortest path between the starting point and the goal point
void search (bool map[][COL], Cell start, Cell goal) {

	Cell arrayCells[ROW*COL];	// Array containining the details of each cell
	
	// Initialization of each cell
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			int pos = i*ROW + j;
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

	Cell *openSet;		// Priority queue
	openSet = (Cell*)malloc(sizeof(Cell)*10);
	int openSetSize = 0;

	Cell *closedSet;	// Cells already visited
	closedSet = (Cell*)malloc(sizeof(Cell)*10);
	int closedSetSize = 0;
	
	Cell *path;		// Path between the starting point and the goal point
	path = (Cell*)malloc(sizeof(Cell)*10);

	openSet[0] = start;		// The first cell in the open set is the starting cell
	openSetSize++;

	if (openSetSize > 0) {
		
		int best = 0;	// Initial assumption: the cell having the lowest value of f is in the first position of the open set

		// Scan the open set to find the new best
		for (int i = 0; i < openSetSize; i++) {
			if (openSet[i].f < openSet[best].f)
				best = i;
		}

		Cell c = openSet[best];		// c is the current cell

		// If the current cell is the goal point, the algorithm stops
		if (is_goal(c.row, c.col, goal)) {
			
			Cell tmp = c;	// Temporary cell initialized to current cell (goal point)
			int i = 0;
			
			path[i] = tmp;	// Adding the cell in the first position of the path
			i++;
			
			// These instructions are executed every time a cell has a parent (the loop stops when the starting cell is evaluated, whose parent is the cell itself) 
			while ((tmp.parentRow && tmp.parentCol) && (tmp.row != tmp.parentRow && tmp.col != tmp.parentCol)) {
				int pos = tmp.parentRow*ROW + tmp.parentCol; // Variable used to find the position of the cell within arrayCells 
				
				// ATTENZIONE: andrebbe inserita una realloc per il vettore path!"
				
				path[i] = arrayCells[pos];	// Adding the parent cell to the path
				tmp = arrayCells[pos];	// Temporary cell value updated to the parent cell 
				i++;
			}
			
			int pathSize = sizeof(*path)/sizeof(path[0]);
			
			printf("Goal reached! The backward path is:\n");
			
			// Print the backward path
			for (int j = 0; j < pathSize; j++) {
				printf("(%d, %d)\n", path[j].row, path[j].col);
			}
			
			free(openSet);
			free(closedSet);
			free(path);
			
			return;
		}
		
		// Removing the current cell from the open set
		for (int i = best; i < openSetSize - 1; i++) {
			openSet[i] = openSet[i + 1];
		}

		openSet = realloc(openSet, (openSetSize - 1)*sizeof(Cell));
		openSetSize--;

		closedSet[closedSetSize] = c; // Adding the current cell inside the closed set
		closedSetSize++;

		//Cell * neighbors;
		
		/*neighbors = find_neighbors(map, current, current.numNeighbors);*/
		/*
			ATTENZIONE: da qui inizia l'ex funzione find_neighbors()
		*/
		
		int counter = 0;
		Cell neighbor;
		Cell tmp[CONNECTIVITY];
	
		// Neighbor in position (row - 1, col)
		if ((check_position(c.row - 1, c.col)) && (is_free(c.row - 1, c.col, map))) {
			neighbor.row = c.row - 1;
			neighbor.col = c.col;
			tmp[counter] = neighbor;
			counter++;
		}

		// Neighbor in position (row, col + 1)
		if ((check_position(c.row, c.col + 1)) && (is_free(c.row, c.col + 1, map))) {
			neighbor.row = c.row;
			neighbor.col = c.col + 1;
			tmp[counter] = neighbor;
			counter++;
		}
	
		// Neighbor in position (row + 1, col)
		if ((check_position(c.row + 1, c.col)) && (is_free(c.row + 1, c.col, map))) {
			neighbor.row = c.row + 1;
			neighbor.col = c.col;
			tmp[counter] = neighbor;
			counter++;
		}
	
		// Neighbor in position (row, col - 1)
		if ((check_position(c.row, c.col - 1)) && (is_free(c.row, c.col - 1, map))) {
			neighbor.row = c.row;
			neighbor.col = c.col - 1;
			tmp[counter] = neighbor;
			counter++;
		}
	
		if (CONNECTIVITY == 8) {
	
			// Neighbor in position (row - 1, col + 1)
			if ((check_position(c.row - 1, c.col + 1)) && (is_free(c.row - 1, c.col + 1, map))) {
				neighbor.row = c.row - 1;
				neighbor.col = c.col + 1;
				tmp[counter] = neighbor;
				counter++;
			}
	
			// Neighbor in position (row + 1, col + 1)
			if ((check_position(c.row + 1, c.col + 1)) && (is_free(c.row + 1, c.col + 1, map))) {
				neighbor.row = c.row + 1;
				neighbor.col = c.col + 1;
				tmp[counter] = neighbor;
				counter++;
			}
	
			// Neighbor in position (row + 1, col - 1)
			if ((check_position(c.row + 1, c.col - 1)) && (is_free(c.row + 1, c.col - 1, map))) {
				neighbor.row = c.row + 1;
				neighbor.col = c.col - 1;
				tmp[counter] = neighbor;
				counter++;
			}
	
			// Neighbor in position (row - 1, col - 1)
			if ((check_position(c.row - 1, c.col -  1)) && (is_free(c.row - 1, c.col - 1, map))) {
				neighbor.row = c.row - 1;
				neighbor.col = c.col - 1;
				tmp[counter] = neighbor;
				counter++;	
			}
		}
	
		Cell neighbors[counter];
		for (int i=0; i<counter; i++){
			neighbors[i] = tmp[i];
			printf("\nsearch: ");
			printf("%d %d, ", neighbors[i].row, neighbors[i].col);
		}
		
		c.numNeighbors = counter;
		printf("\nsearch: %d neighbors", c.numNeighbors);
		
		/*
			FINE DELLA EX FUNZIONE FIND_NEIGHBORS()
		*/
		
		int neighborSize = sizeof(*neighbors)/sizeof(neighbors[0]);
		//printf("\nsearch: %d / %d = %d", sizeof(*neighbors), sizeof(neighbors[0]), neighborSize);
		//printf("search: %d %d, %d %d, %d %d, %d %d - size: %d - size: %d", neighbors[0].row, neighbors[0].col, neighbors[1].row, neighbors[1].col, neighbors[2].row, neighbors[2].col, neighbors[3].row, neighbors[3].col, sizeof(neighbors), sizeof(neighbors[0]));

		/*
		// for loop (PROVA) - da eliminare
		for(int i=0; i<c.numNeighbors; i++){
			printf("\n %d", neighbors[i].row);
		}
		*/
		
		// Loop for checking every neighbor of the current cell
		for (int i = 0; i < neighborSize; i++) {
			Cell neighbor = neighbors[i];

			// Check if the neighbor is already in the closed set. If the negighbor is already
			// in the closed set it is evaluated, otherwise nothing is done
			for (int j = 0; j < closedSetSize; j++) {
				if (neighbor.row != closedSet[j].row && neighbor.col != closedSet[j].col) {
					if (j == closedSetSize - 1) {
						
						// If I am here the neighbor is not in the closed set
						
						// Here we have to add +1 or +sqrt(2)
						double tmpG;
						/*if(c.row==neighbor.row || c.col==neighbor.col)
							tmpG = c.g + 1;
						else
							tmpG = c.g + sqrt(2);*/
						tmpG = c.g + heuristic(c, neighbor);
						printf("distance: %f", tmpG);

						// Check if the neighbor is already in the open set. If it is not it means that a new cell was discovered
						for (int k = 0; k < openSetSize; k++) {
							if (neighbor.row == openSet[k].row && neighbor.col == openSet[k].col) {

									// If I am here the neighbor is already in the open set

									// Check if the neighbor has been reached with a lower cost than before. 
									// If yes, its value of g is updated, otherwise nothing is done 
									if (tmpG < neighbor.g)
										neighbor.g = tmpG;
							}
							else if (k == openSetSize - 1) {

								// If I am here the neighbor is not in the open set

								neighbor.g = tmpG;

								// ATTENZIONE: Qui va inserito "neighbor" all'interno dell'open set + la realloc per l'open set
							}
						}

						neighbor.h = heuristic(neighbor, goal);
						neighbor.f = neighbor.g + neighbor.h;
						neighbor.parentRow = c.row;
						neighbor.parentCol = c.col;
					}
				}
			}
		}
	} else {
		printf("No solution was found\n");
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
		{true, true, true, true, true, true, true}
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