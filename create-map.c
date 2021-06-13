#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define DIM 1000		// Dimension of rows and columns of the map
#define OBSTACLES 10	// Percentage of obstacles in the map
#define SEED 0

/* This function is used to create a map of a given dimension and a given percentage of obstacles. If the percentage of obstacles is greater
than zero, then the cells within the map are marked as free [true] or occupied [false], otherwise, if it is equal to zero, all cells are marked
as free. Finally, an error message is reported if the percentage of obstacles is less than zero. */
bool createMap(bool map[]){
	int r,c,elem;
	int free_cell_num = 0;
	int obstPercent = OBSTACLES;
	
	printf("Filling map... ");
	
	/* If we enter this if() statement, then the percentage of obstacles is greater than zero. Consequently, a certain number of cells, proportional
	to the percentage of obstacles, will be indicated as occupied [false], while the rest will be indicated as free [true]. */
	if(obstPercent > 0){
		int obst = 10000/obstPercent;		// obst = 10000/obstPercent is an obstacle
		srand(SEED);						// Not casual		
		
		for (r=0; r<DIM; r++){
			for (c=0; c<DIM; c++){
				elem=rand()%obst;			// Pseudo-random integer in the range [0, obst]
				if (elem < 100){
					map[r*DIM+c] = false;	// Cell occupied, i.e., there is an obstacle
				} else {
					map[r*DIM+c] = true;	// Cell free
					free_cell_num++;
				}
			}
		}	
	} 
	/* If we enter this else if() statement, then the percentage of obstacles is equal to zero. Consequently, all cells are marked as free [true]. */
	else if (obstPercent == 0) {			// No obstacles
		free_cell_num = DIM*DIM;
		
		for (r=0; r<DIM; r++){
			for (c=0; c<DIM; c++){
				map[r*DIM+c] = true;		// Cell free
			}
		}
	} 
	/* If we enter this else statement, then the percentage of obstacles is less than zero. Consequently, an error message is reported. */
	else {
		printf("\nError! The percentage of obstacles must be greater than or equal to zero.\n");
		return false;
	}
	
	printf("Map %dx%d filled with %d free cells.\n", DIM, DIM, free_cell_num);
	return true;
}

/* Main function. Here the space needed for the map is allocated. Subsequently, if the creation of the map was successful, a file is created in 
the "maps" folder in which the map is printed. */
int main(){
	bool *map;
	map = (bool *)malloc(DIM*DIM*sizeof(bool));
	
	if(map == NULL) {
		printf("\nFailed allocation of the map.\n");
		exit(1);
	}

	if(!createMap(map)){
		exit(1);
	} else {
		FILE *fp;
		char fname[40];
		sprintf(fname,"./maps/map-dim%d-obst%d",DIM,OBSTACLES);
		fp = fopen(fname,"w");
		
		if (fp == NULL){
			printf("Error opening file.\n");
			exit(1);
		}
		
		printf("Printing the map... ");		
		int r,c;
		
		for(r=0; r<DIM; r++){
			for(c=0; c<DIM; c++){
				fprintf(fp, "%d ", map[r*DIM+c]);
			}
			fprintf(fp,"\n");
		}
		
		printf("The map has been printed. You can find the file \"map-dim%d-obst%d\" inside the \"maps\" folder.\n",DIM,OBSTACLES);
		fclose(fp);
		printf("File closed successfully.\n");
	}
	return 0;
}
