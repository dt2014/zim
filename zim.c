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
    
	object **MeshA = CreateMesh(SIZE + 2, SIZE + 2);
	object **MeshB = CreateMesh (SIZE + 2, SIZE + 2);
	for (int i = 1; i <= SIZE; i++) {
		for (int j = 1; j <= SIZE; j++) {
			if (drand48() < 0.1) {
				MeshA[i][j] = (object) allocate(sizeof(struct Object));
                MeshA[i][j]->gender = setGender();				
				MeshA[i][j]->age = rand()%99+1;
				MeshA[i][j]->type = 'H';
			}
		}
	}
    
	printMeshHeading(MeshA, 1);
	printMesh(MeshA);
    
	for (int n = 0; n < STEPS; n++) {
        
        /* 1. MOVE */
        #if defined(_OPENMP) 
        double startTime = omp_get_wtime();
		#pragma omp parallel for default(none) shared(MeshA,MeshB,locks,n)
		#endif
		for (int i = 1; i <= SIZE; i++) {
			#if defined(_OPENMP)
			lockForMove(i, locks);
			#endif
			for (int j = 1; j <= SIZE; j++) 
                if (MeshA[i][j] != NULL) {
                    double move = drand48();
                    if (move < 1.0*MOVE && MeshA[i-1][j] == NULL && MeshB[i-1][j] == NULL) {
                        MeshB[i-1][j] = MeshA[i][j];
                    } else if (move < 2.0*MOVE && MeshA[i+1][j] == NULL && MeshB[i+1][j] == NULL) {
                        MeshB[i+1][j] = MeshA[i][j];
                    } else if (move < 3.0*MOVE && MeshA[i][j-1] == NULL && MeshB[i][j-1] == NULL) {
                        MeshB[i][j-1] = MeshA[i][j];
                    } else if (move < 4.0*MOVE && MeshA[i][j+1] == NULL && MeshB[i][j+1] == NULL) {
                        MeshB[i][j+1] = MeshA[i][j];
                    } else {
                        MeshB[i][j] = MeshA[i][j];
                    }
                    MeshA[i][j] = NULL;
                } 
			#if defined(_OPENMP)
			unlockForMove(i, locks);
			#endif
		}
        /* 2. BOUNDARY CHECK AFTER MOVE */
		MeshB = ScanOutOfRange(MeshB);
        swap(&MeshA, &MeshB);
        
        /* 3. DEATH */
        #if defined(_OPENMP)
		#pragma omp parallel for default(none) shared(MeshA,MeshB,n)
		#endif
        for (int i = 1; i <= SIZE; i++) {
			for (int j = 1; j <= SIZE; j++) 
                if (MeshA[i][j] != NULL) { 
                    double death = drand48();
                    if (death > DEATH) {
                        MeshB[i][j] = MeshA[i][j];
                    } else {
                        free(MeshA[i][j]);
                    }
                    MeshA[i][j] = NULL;
                }
		}
        swap(&MeshA, &MeshB);
        
        /* 4. BIRTH */
        #if defined(_OPENMP)
		#pragma omp parallel for default(none) shared(MeshA,MeshB,locks,n)
		#endif
		for (int i = 1; i < SIZE; i++) {
            #if defined(_OPENMP)
			lockForPair(i, locks);
			#endif
			for (int j = 1; j < SIZE; j++) 
                if (MeshA[i][j] != NULL) {                    
                    double birth = drand48();
                    if (birth < BIRTH && MeshA[i+1][j] != NULL && canReproduce(MeshA[i][j], MeshA[i+1][j])) {
                        MeshB[i+1][j] = MeshA[i+1][j];
                        MeshA[i+1][j] = NULL;
                        
                        object baby = (object) allocate(sizeof(struct Object));
                        baby->gender = setGender();
                        baby->age = 0;
                        baby->type = 'H';
                        
                        if (MeshA[i-1][j] == NULL && MeshB[i-1][j] == NULL) {
                            MeshB[i-1][j] = baby;
                        } else if (MeshA[i][j-1] == NULL && MeshB[i][j-1] == NULL) {
                            MeshB[i][j-1] = baby;
                        } else if (MeshA[i+1][j-1] == NULL && MeshB[i+1][j-1] == NULL) {
                            MeshB[i+1][j-1] = baby;
                        } else if (MeshA[i][j+1] == NULL && MeshB[i][j+1] == NULL) {
                            MeshB[i][j+1] = baby;
                        } else if (MeshA[i+1][j+1] == NULL && MeshB[i+1][j+1] == NULL) {
                            MeshB[i+1][j+1] = baby;
                        } else if (MeshA[i+2][j] == NULL && MeshB[i+2][j] == NULL) {
                            MeshB[i+2][j] = baby;
                        } else { // no empty spot for a baby
                            free(baby);
                        }
                        
                    } else if (birth < BIRTH && MeshA[i][j+1] != NULL && canReproduce(MeshA[i][j], MeshA[i][j+1])) {
                        MeshB[i][j+1] = MeshA[i][j+1];
                        MeshA[i][j+1] = NULL;
                        
                        object baby = (object) allocate(sizeof(struct Object));
                        baby->gender = setGender();
                        baby->age = 0;
                        baby->type = 'H';
                        
                        if (MeshA[i][j-1] == NULL && MeshB[i][j-1] == NULL) {
                            MeshB[i][j-1] = baby;
                        } else if (MeshA[i-1][j] == NULL && MeshB[i-1][j] == NULL) {
                            MeshB[i-1][j] = baby;
                        } else if (MeshA[i-1][j+1] == NULL && MeshB[i-1][j+1] == NULL) {
                            MeshB[i-1][j+1] = baby;
                        } else if (MeshA[i+1][j] == NULL && MeshB[i+1][j] == NULL) {
                            MeshB[i+1][j] = baby;
                        } else if (MeshA[i+1][j+1] == NULL && MeshB[i+1][j+1] == NULL) {
                            MeshB[i+1][j+1] = baby;
                        } else if (MeshA[i][j+2] == NULL && MeshB[i][j+2] == NULL) {
                            MeshB[i][j+2] = baby;
                        } else { // no empty spot for a baby
                            free(baby);
                        }
                    }
                    MeshB[i][j] = MeshA[i][j];
                    MeshA[i][j] = NULL;
                }
            #if defined(_OPENMP)
			unlockForPair(i, locks);
			#endif
		}
		swap(&MeshA, &MeshB);
        
        
        
        
		printMeshHeading(MeshA, n + 1);
		//printMesh(MeshA);
        #if defined(_OPENMP)
        printf("Max_threads: %d, time: %f\n\n\n", omp_get_max_threads(), omp_get_wtime() - startTime);
		#endif
	}
    //printMesh(MeshA);
    return 0;
}