/* 
 * File:   mesh.c
 * Author: 
 *
 * Created on September 15, 2014, 12:52 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include "mesh.h"
#include "paras.h"

Spot **CreateMesh(int I, int J) {
    Spot **Mesh = (Spot**) allocate(I * sizeof(Spot*));
	for (int i = 0; i < I; i++) {
			Mesh[i] = (Spot*) allocate(J * sizeof(Spot));
		for (int j = 0; j < J; j++) 
			{
				Mesh[i][j].taken = FALSE;
				Mesh[i][j].object = NULL;
			}
	}
	return Mesh;
}

Spot **ScanOutOfRange(Spot **Mesh) {
	for(int i = 1; i <= SIZE; i++){
		if (Mesh[i][0].taken == TRUE) 
		{
			Mesh[i][0].taken = FALSE;
			Mesh[i][1].taken = TRUE;
			Mesh[i][1].object = Mesh[i][0].object;
			Mesh[i][0].object = NULL;
			
		}
		if (Mesh[i][SIZE+1].taken == TRUE)
		{
			Mesh[i][SIZE+1].taken = FALSE;
			Mesh[i][SIZE].taken = TRUE;
			Mesh[i][SIZE].object = Mesh[i][SIZE+1].object;
			Mesh[i][SIZE+1].object = NULL;
		}
	}
	for(int j = 1; j <= SIZE; j++){
		if(Mesh[0][j].taken == TRUE) 
		{
			Mesh[0][j].taken = FALSE;
			Mesh[1][j].taken = TRUE;
			Mesh[1][j].object = Mesh[0][j].object;
			Mesh[0][j].object = NULL;
		}
		if(Mesh[SIZE+1][j].taken == TRUE)
		{
			Mesh[SIZE+1][j].taken = FALSE;
			Mesh[SIZE][j].taken = TRUE;
			Mesh[SIZE][j].object = Mesh[SIZE+1][j].object;
			Mesh[SIZE+1][j].object = NULL;			
		}
	}
	return Mesh;
}

void swap(Spot*** a, Spot*** b){
    Spot **tmp = *a;  
    *a = *b;  
    *b = tmp;  
}





void printMeshHeading(Spot **Mesh, int t) {
	int populationTotal = 0;
	for (int i = 1; i <= SIZE; i++) {
		for (int j = 1; j <= SIZE; j++) {
			if (Mesh[i][j].taken == TRUE) populationTotal++;
		} 
	}
    printf("%s: %d, population: %d\n", DELTA_T, t, populationTotal);
}

void printMesh(Spot **Mesh) {
    /*#if defined(_OPENMP) 
	#pragma omp parallel for default(none) shared(cout, Mesh)
    #endif*/
	for (int i = 0; i <= SIZE+1; i++) {
        printf("----------------------------------------------------------------------\n");
		for (int j = 0; j <= SIZE+1; j++) {
			if (Mesh[i][j].taken == TRUE)
			{
				if(Mesh[i][j].object->age < 10)
                    printf("|%c0%d", Mesh[i][j].object->gender, Mesh[i][j].object->age);
				else
                    printf("|%c%d", Mesh[i][j].object->gender, Mesh[i][j].object->age);
			}
			else
                printf("|   ");
		}
		printf("|\n");
	}
	printf("----------------------------------------------------------------------\n\n");
}

