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
                MeshA[i][j].object->gender = getGender();				
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

        /* 1. MOVE 
		for (int i = 1; i <= SIZE; i++) {
            
			#if defined(_OPENMP)
			lockForMove(i, locks);
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
			unlockForMove(i, locks);
			#endif

		}*/
        
        /* 2. BOUNDARY CHECK 
		MeshB = ScanOutOfRange(MeshB);
        swap(&MeshA, &MeshB);*/
        
        /* 3. DEATH */
        for (int i = 1; i <= SIZE; i++) {
			for (int j = 1; j <= SIZE; j++) 
                if (MeshA[i][j].taken == TRUE) { 
                    MeshA[i][j].taken = FALSE;
                    Object *temp = MeshA[i][j].object;
                    MeshA[i][j].object = NULL;
                    double death = drand48();
                    if (death > DEATH) {
                        MeshB[i][j].taken = TRUE;
                        MeshB[i][j].object = temp;
                    } else {
                        free(temp);
                    }
                }
		}
        swap(&MeshA, &MeshB);
        
        /* 4. BIRTH */
		for (int i = 1; i < SIZE; i++) {
            #if defined(_OPENMP)
			lockForPair(i, locks);
			#endif
			for (int j = 1; j < SIZE; j++) 
                if (MeshA[i][j].taken == TRUE) {                    
                    double birth = drand48();
                    if (birth < BIRTH && MeshA[i+1][j].taken == TRUE && canReproduce(MeshA[i][j].object, MeshA[i+1][j].object)) {
                        MeshB[i+1][j].taken = TRUE;
                        MeshB[i+1][j].object = MeshA[i+1][j].object;
                        MeshA[i+1][j].taken = FALSE;
                        MeshA[i+1][j].object = NULL;
                        
                        Object *baby = (Object*) allocate(sizeof(Object));
                        baby->gender = getGender();
                        baby->age = 0;
                        baby->type = 'H';
                        
                        if (i != 1 && MeshA[i-1][j].taken == FALSE && MeshB[i-1][j].taken == FALSE) {
                            MeshB[i-1][j].taken = TRUE;
                            MeshB[i-1][j].object = baby;
                        } else if (j != 1 && MeshA[i][j-1].taken == FALSE && MeshB[i][j-1].taken == FALSE) {
                            MeshB[i][j-1].taken = TRUE;
                            MeshB[i][j-1].object = baby;
                        } else if (j != 1 && MeshA[i+1][j-1].taken == FALSE && MeshB[i+1][j-1].taken == FALSE) {
                            MeshB[i+1][j-1].taken = TRUE;
                            MeshB[i+1][j-1].object = baby;
                        } else if (MeshA[i][j+1].taken == FALSE && MeshB[i][j+1].taken == FALSE) {
                            MeshB[i][j+1].taken = TRUE;
                            MeshB[i][j+1].object = baby;
                        } else if (MeshA[i+1][j+1].taken == FALSE && MeshB[i+1][j+1].taken == FALSE) {
                            MeshB[i+1][j+1].taken = TRUE;
                            MeshB[i+1][j+1].object = baby;
                        } else if (i+1 != SIZE && MeshA[i+2][j].taken == FALSE && MeshB[i+2][j].taken == FALSE) {
                            MeshB[i+2][j].taken = TRUE;
                            MeshB[i+2][j].object = baby;
                        } else { // no empty spot for a baby
                            free(baby);
                        }
                        
                    } else if (birth < BIRTH && MeshA[i][j+1].taken == TRUE && canReproduce(MeshA[i][j].object, MeshA[i][j+1].object)) {
                        MeshB[i][j+1].taken = TRUE;
                        MeshB[i][j+1].object = MeshA[i][j+1].object;
                        MeshA[i][j+1].taken = FALSE;
                        MeshA[i][j+1].object = NULL;
                        
                        Object *baby = (Object*) allocate(sizeof(Object));
                        baby->gender = getGender();
                        baby->age = 0;
                        baby->type = 'H';
                        
                        if (j != 1 && MeshA[i][j-1].taken == FALSE && MeshB[i][j-1].taken == FALSE) {
                            MeshB[i][j-1].taken = TRUE;
                            MeshB[i][j-1].object = baby;
                        } else if (i != 1 && MeshA[i-1][j].taken == FALSE && MeshB[i-1][j].taken == FALSE) {
                            MeshB[i-1][j].taken = TRUE;
                            MeshB[i-1][j].object = baby;
                        } else if (i != 1 && MeshA[i-1][j+1].taken == FALSE && MeshB[i-1][j+1].taken == FALSE) {
                            MeshB[i-1][j+1].taken = TRUE;
                            MeshB[i-1][j+1].object = baby;
                        } else if (MeshA[i+1][j].taken == FALSE && MeshB[i+1][j].taken == FALSE) {
                            MeshB[i+1][j].taken = TRUE;
                            MeshB[i+1][j].object = baby;
                        } else if (MeshA[i+1][j+1].taken == FALSE && MeshB[i+1][j+1].taken == FALSE) {
                            MeshB[i+1][j+1].taken = TRUE;
                            MeshB[i+1][j+1].object = baby;
                        } else if (j+1 != SIZE && MeshA[i][j+2].taken == FALSE && MeshB[i][j+2].taken == FALSE) {
                            MeshB[i][j+2].taken = TRUE;
                            MeshB[i][j+2].object = baby;
                        } else { // no empty spot for a baby
                            free(baby);
                        }
                    }
                    MeshB[i][j].taken = TRUE;
                    MeshB[i][j].object = MeshA[i][j].object;
                    MeshA[i][j].taken = FALSE;
                    MeshA[i][j].object = NULL;
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