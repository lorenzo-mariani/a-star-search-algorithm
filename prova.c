#include <stdio.h>
#include <stdlib.h>

int main() {
	
	int *v;	// Cells already visited
	v = (int*)malloc(sizeof(int));
	int dim = 1;
	
	v = realloc(v, (dim - 1)*sizeof(int));
	
}
