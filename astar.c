#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define ROW 7 
#define COL 7
#define CONNECTIVITY 8

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

// Search for cells adjacent to the current one
Cell * find_neighbors (bool map[][COL], Cell c) {

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
	}

	return neighbors;
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

// Function to find the shortest path between the starting point and the goal point
void search (bool map[][COL], Cell start, Cell goal) {

	Cell arrayCells[ROW*COL];	// Array containining the details of each cell
	
	// Initialization of each cell
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			int pos = i*ROW + j;
			arrayCells[pos].f = 10000.0;
			arrayCells[pos].g = 10000.0;
			arrayCells[pos].h = 10000.0;
			arrayCells[pos].parentRow = -1;
			arrayCells[pos].parentCol = -1;
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
	openSet = (Cell*)malloc(sizeof(Cell)*10);
	int openSetSize = 0;

	Cell *closedSet;	// Cells already visited
	closedSet = (Cell*)malloc(sizeof(Cell)*10);
	int closedSetSize = 0;

	//Cell neighbors[CONNECTIVITY];	// Neighbors of a given cell

	openSet[0] = start;	// The first cell in the open set is the starting cell
	openSetSize++;

	if (openSetSize > 0) {
		
		int best = 0;	// Assumption: the cell having the lowest value of f is in the first position of the open set

		// Scan the open set to find the new best
		for (int i = 0; i < openSetSize; i++) {
			if (openSet[i].f < openSet[best].f)
				best = i;
		}

		Cell current = openSet[best];

		// If the current cell is the goal point the algorithm stops
		if (is_goal(current.row, current.col, goal)) {
			free(openSet);
			free(closedSet);
			//free(neighbors);
			return;
		}
		
		// Removing the current cell from the open set
		for (int i = best; i < openSetSize - 1; i++) {
			openSet[i] = openSet[i + 1];
		}

		openSet = realloc(openSet, (openSetSize - 1)*sizeof(Cell));

		closedSet[closedSetSize] = current; // Adding the current cell inside the closed set
		closedSetSize++;
		openSetSize--;

		Cell * neighbors;
		neighbors = find_neighbors(map, current);
		int neighborSize = sizeof(neighbors)/sizeof(Cell);

		for (int i = 0; i < neighborSize; i++) {
			Cell neighbor = neighbors[i];

			for (int j = 0; j < closedSetSize; j++) {
				if (neighbor.row != closedSet[j].row && neighbor.col != closedSet[j].col) {
					if (j == closedSetSize - 1) {

						double tmpG = current.g + 1;

						for (int k = 0; k < openSetSize; k++) {
							if (neighbor.row == openSet[k].row && neighbor.col == openSet[k].col) {
								if (k == openSetSize - 1) {
									if (tmpG < neighbor.g)
										neighbor.g = tmpG;
								}
							}
							else if (k == openSetSize - 1) {
								neighbor.g = tmpG;
								// Here the "neighbor" must be added to the open set + realloc
							}
						}

						neighbor.h = heuristic(neighbor, goal);
						neighbor.f = neighbor.g + neighbor.h;
						neighbor.parentRow = current.row;
						neighbor.parentCol = current.col;
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
	}
	else {
		return 0;
	}

	return 1;
}
