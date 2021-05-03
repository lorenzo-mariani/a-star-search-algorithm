#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define DIM 3
#define OBSTACLES 30
#define SEED 0

bool createMap(bool map[]){
	int r,c,elem;
	int free_cell_num = 0;
	int obstPercent = OBSTACLES;
	printf("Filling map... ");
	if(obstPercent != 0){
		int obst = 10000/obstPercent;	// obst = 10000/obstPercent is an obstacle
		srand(SEED);					// not casual		
		
		for (r=0; r<DIM; r++){
			for (c=0; c<DIM; c++){
				elem=rand()%obst;				// pseudo-random integer in the range [0, obst]
				if (elem < 100){
					map[r*DIM+c] = false;		// the cell is not free
				} else {
					map[r*DIM+c] = true;		// the cell is free
					free_cell_num++;
				}
			}
		}
		// start and goal points assumed as always free
		//map[start[0]*DIM+start[1]] = true;
		//map[goal[0]*DIM+goal[1]] = true;
		
	} else {							// no obstacles
		free_cell_num = DIM*DIM;
		
		for (r=0; r<DIM; r++){
			for (c=0; c<DIM; c++){
				map[r*DIM+c] = true;		// free
			}
		}
	}
	printf("Map %dx%d filled with %d free cells.\n", DIM, DIM, free_cell_num);
	return true;
}

int main(){
	
	FILE *fp;
	
	bool *map;
	map = (bool *)malloc(DIM*DIM*sizeof(bool));
	if(map == NULL) {
		printf("\nFailed allocation.\n");
		exit(1);
	}
	
	char fname[40], buffer[5];
	strcpy(fname,"./maps/map-dim");
	strcat(fname,itoa(DIM,buffer,10));
	strcat(fname,"-obst");
	strcat(fname,itoa(OBSTACLES,buffer,10));
	
	fp = fopen(fname,"w");
	if (fp == NULL){
		printf("Errore nell'apertura del file.\n");
		exit(1);
	} else {
		if(!createMap(map)){
			exit(1);
		}
				
		int r,c;
		for(r=0; r<DIM; r++){
			for(c=0; c<DIM; c++){
				fprintf(fp, "%d ", map[r*DIM+c]);
			}
			fprintf(fp,"\n");
		}
		
		fclose(fp);
		printf("File chiuso correttamente.\n");
	}
	return 0;
}
