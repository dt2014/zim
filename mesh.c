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

object **CreateMesh(int I, int J) {
    object **Mesh = (object**) allocate(I * sizeof(object*));
	for (int i = 0; i < I; i++) {
			Mesh[i] = (object*) allocate(J * sizeof(object));
		for (int j = 0; j < J; j++) {
				Mesh[i][j] = NULL;
			}
	}
	return Mesh;
}

object **ScanOutOfRange(object **Mesh) {
	for(int i = 1; i <= SIZE; i++) {
		if (Mesh[i][0] != NULL) {
			Mesh[i][1] = Mesh[i][0];
			Mesh[i][0] = NULL;
			
		}
		if (Mesh[i][SIZE+1] != NULL) {
			Mesh[i][SIZE] = Mesh[i][SIZE+1] ;
			Mesh[i][SIZE+1] = NULL;
		}
	}
	for(int j = 1; j <= SIZE; j++){
		if(Mesh[0][j] != NULL) {
			Mesh[1][j] = Mesh[0][j];
			Mesh[0][j] = NULL;
		}
		if(Mesh[SIZE+1][j] != NULL) {
			Mesh[SIZE][j] = Mesh[SIZE+1][j];
			Mesh[SIZE+1][j] = NULL;			
		}
	}
	return Mesh;
}

void swap(object*** a, object*** b){
    object **tmp = *a;  
    *a = *b;  
    *b = tmp;  
}

void addDemographicNbr(int *demographic, object **Mesh, int t) {
	int hQty = 0;
    int zQty = 0;
    #if defined(_OPENMP)
	#pragma omp parallel for default(none) shared(Mesh) reduction(+:hQty,zQty)
	#endif
	for (int i = 1; i <= SIZE; i++) {
		for (int j = 1; j <= SIZE; j++) {
			if (Mesh[i][j] != NULL) {
                switch(Mesh[i][j]->type) {
                        case 'H': hQty++; break;
                        default: zQty++;
                }
            }
		} 
	}
    demographic[2*t] = hQty;
    demographic[2*t+1] = zQty;
    //printf("%s: %d, Human: %d, Zombie: %d\n", DELTA_T, t, hQty, zQty);
}


void printMesh(object **Mesh) {
	for (int i = 0; i <= SIZE+1; i++) {
        printf("----------------------------------------------------------------------\n");
		for (int j = 0; j <= SIZE+1; j++) {
			if (Mesh[i][j] != NULL) {
				if(Mesh[i][j]->age < 10)
                    printf("|%c0%d", Mesh[i][j]->type == 'H' ? Mesh[i][j]->gender : 'Z', Mesh[i][j]->age);
				else
                    printf("|%c%d", Mesh[i][j]->type == 'H' ? Mesh[i][j]->gender : 'Z', Mesh[i][j]->age);
			}
			else
                printf("|   ");
		}
		printf("|\n");
	}
	printf("----------------------------------------------------------------------\n\n");
}

