#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define ROW 7 
#define COL 7
#define MAX_NEIGHBORS 8

// Position and paramaters of a cell in a map
typedef struct {
	int row, col;
	double f, g, h;
} Cell;

/*
// Computation of heuristic via Manhattan distance (to be used if there is 4-point connectivity) 
double heuristic (Cell a, Cell b) {
	return ((double)((abs(a.row – b.row))+(abs(a.col - b.col))));
}

// Computation of heuristic via Euclidean distance (to be used if there is 8-point connectivity)
double heuristic (Cell a, Cell b) {
	return ((double)sqrt((a.row - b.row)*(a.row - b.row) + (a.col - b.col)*(a.col - b.col)));
}
*/

// Check whether the position of a cell complies with the dimension of a map or not
bool check_position (int row, int col) {
	if (row >= 0 && row < ROW && col >= 0 && col < COL)
		return true;
	else
		return false; 
}

// Check whether the goal point has been reached or not
bool is_goal (int row, int col, Cell goal) {
	if (row == goal.row && col == goal.col)
		return true;
	else
		return false;
}

// Check if the current cell is free or if there is an obstacle
bool is_free (int row, int col, int map[][COL]) {
	if (map[row][col] == 1)
		return true;
	else
		return false;
}

// Check the correctness of the start and goal cells
int check (Cell start, Cell goal, int map[][COL]) {
	if ((!check_position(start.row, start.col)) || (!check_position(goal.row, goal.col))) {
		printf("Start/goal point out of the map\n");
		return 0;
	}
	
	if ((!is_free(start.row, start.col, map)) || (!is_free(goalrow, goal.col, map))) {
		printf("Start/goal point not reachable\n");
		return 0;
	}

	if (is_goal(start.row, start.col, goal)) {
		printf("Start and goal points are the same\n");
		return 0;
	}

	return 1;
}

void find_neighbors (int map[][COL], Cell c, Cell* neighbors) {
	
	int i = 0;
	Cell neighbor;
	
	// Neighbor in position (row - 1, col)
	if (check_position(c.row - 1, c.col)) {
		neighbor.row = c.row - 1;
		neighbor.col = c.col;
		neighbors[i] = neighbor;
		i++;
	}

	// Neighbor in position (row - 1, col + 1)
	if (check_position(current.row - 1, current.col + 1)) {
		neighbor.row = c.row - 1;
		neighbor.col = c.col + 1;
		neighbors[i] = neighbor;
		i++;
	}

	// Neighbor in position (row, col + 1)
	if (check_position(current.row, current.col + 1)) {
		neighbor.row = c.row;
		neighbor.col = c.col + 1;
		neighbors[i] = neighbor;
		i++;
	}

	// Neighbor in position (row + 1, col + 1)
	if (check_position(current.row + 1, current.col + 1)) {
		neighbor.row = c.row + 1;
		neighbor.col = c.col + 1;
		neighbors[i] = neighbor;
		i++;
	}

	// Neighbor in position (row + 1, col)
	if (check_position(current.row + 1, current.col)) {
		neighbor.row = c.row + 1;
		neighbor.col = c.col;
		neighbors[i] = neighbor;
		i++;
	}

	// Neighbor in position (row + 1, col - 1)
	if (check_position(current.row + 1, current.col - 1)) {
		neighbor.row = c.row + 1;
		neighbor.col = c.col - 1;
		neighbors[i] = neighbor;
		i++;
	}

	// Neighbor in position (row, col - 1)
	if (check_position(current.row, current.col - 1)) {
		neighbor.row = c.row;
		neighbor.col = c.col - 1;
		neighbors[i] = neighbor;
		i++;
	}

	// Neighbor in position (row - 1, col - 1)
	if (check_position(current.row - 1, current.col -  1)) {
		neighbor.row = c.row - 1;
		neighbor.col = c.col - 1;
		neighbors[i] = neighbor;
		i++;	
	}

	neighbors = realloc(neighbors, (i)*sizeof(Cell));
	return;
}

// Function to find the shortest path between the starting point and the goal point.
void search (int map[][COL], Cell start, Cell goal) {

	Cell* openSet[] = (Cell*)malloc(sizeof(Cell)*10);
	Cell* closedSet[] = (Cell*)malloc(sizeof(Cell)*10);
	Cell* neighbors[] = (Cell*)malloc(sizeof(Cell)*MAX_NEIGHBORS);

	int openSetSize = 0;
	int closedSetSize = 0;

	openSet[0] = start;
	openSetSize++;

	if (openSetSize > 0) {
		
		int best = 0;

		for (int i = 0; i < openSetSize; i++) {
			if (openSet[i].f < openSet[best].f)
				best = i;
		}

		Cell current = openSet[best];

		if (current == goal) {
			// The algorithm stops
		}
		
		// Removing "current" from the open set
		for (int i = best; i < openSetSize - 1; i++) {
			openSet[i] = openSet[i + 1];
		}
		openSet = realloc(openSet, (openSetSize - 1)*sizeof(Cell));

		closedSet[closedSetSize] = current;
		closedSetSize++;
		openSetSize--;

		find_neighbors(map, current, neighbors);

	} else {
		// No solution
	}
	
	free(openSet);
	free(closedSet);
	free(neighbors);

	return;
}

int main () {
	
	// 0 - There is an obstacle
	// 1 - The cell is free
	int map[ROW][COL] = 
	{
		{1, 1, 1, 1, 1, 1, 1},
		{1, 1, 0, 0, 1, 1, 1},
		{1, 1, 0, 0, 1, 1, 1},
		{1, 0, 0, 0, 1, 1, 1},
		{1, 1, 0, 0, 1, 1, 1},
		{1, 1, 1, 1, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1}
	};

	Cell start;	// Starting point
	Cell goal;	// Goal point

	start.row = 6; 
	start.col = 2;

	goal.row = 1;
	goal.col = 6;

	if (check(start, goal, map[][COL]) == 1) {
		// Execute the algorithm
	}
	else {
		return 0;
	}

	return 1;
}
