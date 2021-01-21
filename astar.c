#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define ROW 7 
#define COL 7

struct cell {

	double g;	// Cost of the backward path from the current cell to the starting point
	double h;	// Heuristic function. Estimates the cost from the current cell to the goal point
	double f;	// f = g + h. Estimates the total cost of the path from the starting point to the goal point passing by the current cell

};

// Used to define the starting point and the goal point
struct key_cell {
	
	int row;
	int col;
};

// Function to compute the heuristic
double heuristic (int row, int col, struct key_cell goal) {
	
	// Euclidean distance
	return ((double)sqrt ((row - goal.row)*(row - goal.row) + (col - goal.col)*(col - goal.col)));
}

// Function to check whether the goal point has been reached or not
bool is_goal (int row, int col, struct key_cell goal) {

	if (row == goal.row && col == goal.col)
		return true;
	else
		return false;

}

// Function to check if the current cell is free or if there is an obstacle
bool is_free(int row, int col, int map[][COL]) {

	if (map[row][col] == 1)
		return true;
	else
		return false;
}

void pathfinding (int map[][COL], struct key_cell start, struct key_cell goal) {

}

int main() {
	
	/*
	0 - There is an obstacle
	1 - The cell is free
	*/

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

	struct key_cell start;	// Starting point
	struct key_cell goal;	// Goal point

	start.row = 6; 
	start.col = 2;

	goal.row = 1;
	goal.col = 6;

	return 0;

}
