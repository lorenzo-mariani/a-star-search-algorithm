#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define ROW 7 
#define COL 7

// Struct to define position and paramaters of a cell in a map
struct Cell {
	int row;
	int col;
	double g;	// Cost of the backward path from the current cell to the starting point
	double h;	// Heuristic function. Estimates the cost from the current cell to the goal point
	double f;	// f = g + h. Estimates the total cost of the path from the starting point to the goal point passing by the current cell
};

/*
// Function to compute the heuristic by using the Manhattan distance.
// Use this function when there is a 4-points connectivity.
int heuristic (struct Cell cell, struct Cell goal) {
	return ((abs(cell.row – goal.row))+(abs(cell.col - goal.col)));
}
*/

/*
// Function to compute the heuristic by using the Euclidean distance.
// Use this function when there is a 8-points connectivity.
double heuristic (struct Cell cell, struct Cell goal) {
	return ((double)sqrt((cell.row - goal.row)*(cell.row - goal.row) + (cell.col - goal.col)*(cell.col - goal.col)));
}
*/

// Function to check whether the position of a cell complies with the dimension of a map or not
bool check_position (struct Cell cell) {
	if (cell.row >= 0 && cell.row < ROW && cell.col >= 0 && cell.col < COL)
		return true;
	else
		return false; 
}

// Function to check whether the goal point has been reached or not
bool is_goal (struct Cell cell, struct Cell goal) {
	if (cell.row == goal.row && cell.col == goal.col)
		return true;
	else
		return false;
}

// Function to check if the current cell is free or if there is an obstacle
bool is_free (struct Cell cell, int map[][COL]) {
	if (map[cell.row][cell.col] == 1)
		return true;
	else
		return false;
}

// Function to check the correctness of the start and goal cells
void control (struct Cell start, struct Cell goal) {
	if ((!check_position(start)) || (!check_position(goal)))
		printf("Start/goal point out of the map\n");

	if ((!is_free(start)) || (!is_free(goal)))
		printf("Start/goal point not reachable\n");

	if (is_goal(start, goal))
		printf("Start and goal points are the same\n");
}

// Function to find the shortest path between the starting point and the goal point.
void pathfinding (int map[][COL], struct Cell start, struct Cell goal) {

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

	struct Cell start;	// Starting point
	struct Cell goal;	// Goal point

	start.row = 6; 
	start.col = 2;

	goal.row = 1;
	goal.col = 6;

	control(start, goal);

	return 0;
}