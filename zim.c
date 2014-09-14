/* 
 * File:   zim.c
 * Author: 
 *
 * Created on September 10, 2014, 9:38 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "mesh.h"
#include "paras.h"

double drand48(void);
void srand48(long int seedval);

/*
 * 
 */
int main(int argc, char** argv) {
    srand48(8767134);
	BOOL *locks = (BOOL*) allocate((SIZE + 2) * sizeof(BOOL*));
	for (int i = 0; i < SIZE + 2; i++)
        locks[i] = FALSE;
	Spot **MeshA = CreateMesh(SIZE + 2, SIZE + 2);
	Spot **MeshB = CreateMesh (SIZE + 2, SIZE + 2);
	for (int i = 1; i <= SIZE; i++) {
		for (int j = 1; j <= SIZE; j++) {
			if (drand48() < 0.1) 
			{	
				MeshA[i][j].taken = TRUE;
				MeshA[i][j].object = (Object*) allocate(sizeof(Object));
				if(rand()%10 >= 5)
					MeshA[i][j].object->gender = 'F';
				else
					MeshA[i][j].object->gender = 'M';
				MeshA[i][j].object->age = rand()%99+1;
				MeshA[i][j].object->type = 'H';
			}
		}
	}
	printMeshHeading(MeshA, 1);
	printMesh(MeshA);
	for (int n = 0; n < STEPS; n++) {
        
		#if defined(_OPENMP) 
        double startTime = omp_get_wtime();
		#pragma omp parallel for default(none) shared(MeshA,MeshB,locks,n)
		#endif

		for (int i = 1; i <= SIZE; i++) {
			#if defined(_OPENMP)
			lock(i, locks);
			#endif
			for (int j = 1; j <= SIZE; j++) 
                if (MeshA[i][j].taken == TRUE) { 
                    MeshA[i][j].taken = FALSE;
                    Object *temp = MeshA[i][j].object;
                    MeshA[i][j].object = NULL;
                    double move = drand48();
                    if (move < 1.0*MOVE && MeshA[i-1][j].taken == FALSE && MeshB[i-1][j].taken == FALSE) {
                        MeshB[i-1][j].taken = TRUE;
                        MeshB[i-1][j].object = temp;
                    } else if (move < 2.0*MOVE && MeshA[i+1][j].taken == FALSE && MeshB[i+1][j].taken == FALSE) {
                        MeshB[i+1][j].taken = TRUE;
                        MeshB[i+1][j].object = temp;
                    } else if (move < 3.0*MOVE && MeshA[i][j-1].taken == FALSE && MeshB[i][j-1].taken == FALSE) {
                        MeshB[i][j-1].taken = TRUE;
                        MeshB[i][j-1].object = temp;
                    } else if (move < 4.0*MOVE && MeshA[i][j+1].taken == FALSE && MeshB[i][j+1].taken == FALSE) {
                        MeshB[i][j+1].taken = TRUE;
                        MeshB[i][j+1].object = temp;
                    } else {
                        MeshB[i][j].taken = TRUE;
                        MeshB[i][j].object = temp;
                    }
                } 
			#if defined(_OPENMP)
			unlock(i, locks);
			#endif
		}
		MeshB = ScanOutOfRange(MeshB);
		swap(&MeshA, &MeshB);
		printMeshHeading(MeshA, n + 1);
		printMesh(MeshA);
        #if defined(_OPENMP)
        printf("Max_threads: %d, time: %f\n\n\n", omp_get_max_threads(), omp_get_wtime() - startTime);
		#endif
	}
    return 0;
}