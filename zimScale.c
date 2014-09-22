/* 
 * File:   zimScale.c
 * This main file is for testing the strong scaling.
 * 
 * Students : 
 * Fengmin Deng     (dengf, 659332)
 * Jiajie Li        (jiajiel, 631482)
 * Shuangchao Yin   (shuangchaoy, 612511) 
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
    
    int MaxThreads = 2;
    
    #if defined(_OPENMP) 
    MaxThreads = omp_get_max_threads();
    
	#endif

    
    for (int t = 1; t <= MaxThreads; t *= 2) {
    #if defined(_OPENMP)
    double startTime = omp_get_wtime();
    omp_set_num_threads(t);
    #endif
	BOOL *locks = createLocks();
    
    #if defined(_OPENMP)
    omp_set_num_threads(t);
    #endif
	object **MeshA = CreateMesh(SIZE + 2, SIZE + 2);
    #if defined(_OPENMP)
    omp_set_num_threads(t);
    #endif
	object **MeshB = CreateMesh (SIZE + 2, SIZE + 2);
    
    // put human into mesh
    #if defined(_OPENMP)
	#pragma omp parallel for default(none) shared(MeshA) num_threads(t)
	#endif
	for (int i = 1; i <= SIZE; i++) {
		for (int j = 1; j <= SIZE; j++) {
			if (drand48() < POP_DENSITY) {
				MeshA[i][j] = (object) allocate(sizeof(struct Object));
                MeshA[i][j]->gender = setGender();				
				MeshA[i][j]->age = rand()%99+1;
				MeshA[i][j]->type = 'H';
			}
		}
	}
    
    // put zombies into mesh
    for (int i = 0; i < INIT_Z_QTY; i++ ) {
        object zombie = (object) allocate(sizeof(struct Object));
        zombie->gender = setGender();				
		zombie->age = rand()%99+1;
		zombie->type = 'Z';
        int cell_i = (int) (drand48() * SIZE);
        int cell_j = (int) (drand48() * SIZE);
        MeshA[cell_i][cell_j] = zombie;
    }
    
    //int *demographic = initDemographic();
    //addDemographicNbr(demographic, MeshA, 0);
	//printMesh(MeshA);
    
    
    
	for (int n = 0; n < STEPS; n++) {
        
        /* 1. MOVE */
        #if defined(_OPENMP)
		#pragma omp parallel for default(none) shared(MeshA,MeshB,locks,n) num_threads(t)
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
        #if defined(_OPENMP)
        omp_set_num_threads(t);
        #endif
		MeshB = ScanOutOfRange(MeshB);
        swap(&MeshA, &MeshB);
        
        /* 3. DEATH */
        #if defined(_OPENMP)
		#pragma omp parallel for default(none) shared(MeshA,MeshB,n) num_threads(t)
		#endif
        for (int i = 1; i <= SIZE; i++) {
			for (int j = 1; j <= SIZE; j++) 
                if (MeshA[i][j] != NULL) { 
                    double death = drand48();
                    if (canAlive(MeshA[i][j], death)) {
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
		#pragma omp parallel for default(none) shared(MeshA,MeshB,locks,n) num_threads(t)
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
                        baby->gender = setBabyGender();
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
                        baby->gender = setBabyGender();
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
        
        /* 5. ZOMBIEFICATION */
        #if defined(_OPENMP)
		#pragma omp parallel for default(none) shared(MeshA,MeshB,locks,n) num_threads(t)
		#endif
		for (int i = 1; i < SIZE; i++) {
            #if defined(_OPENMP)
			lockForPair(i, locks);
			#endif
			for (int j = 1; j < SIZE; j++) 
                if (MeshA[i][j] != NULL) {                    
                    double infect = drand48();
                    double updatedProbOfInfect = n < 365 ? INFECT_EARLY : INFECT_LATER;
                    if ((infect < updatedProbOfInfect) && MeshA[i+1][j] != NULL && canInfect(MeshA[i][j], MeshA[i+1][j])) { 
                        zombiefication(MeshA[i][j], MeshA[i+1][j]);
                        MeshB[i+1][j] = MeshA[i+1][j];
                        MeshA[i+1][j] = NULL;
                    } else if ((infect < updatedProbOfInfect) && MeshA[i][j+1] != NULL && canInfect(MeshA[i][j], MeshA[i][j+1])) {
                        zombiefication(MeshA[i][j], MeshA[i][j+1]);
                        MeshB[i][j+1] = MeshA[i][j+1];
                        MeshA[i][j+1] = NULL;
                    }
                    MeshB[i][j] = MeshA[i][j];
                    MeshA[i][j] = NULL;
                }
            #if defined(_OPENMP)
			unlockForPair(i, locks);
			#endif
		}
		swap(&MeshA, &MeshB);
        
        //addDemographicNbr(demographic, MeshA, n+1);
	}
    
    //printDemographic(demographic);
    #if defined(_OPENMP)
    printf("WorkingThreads: %d, Max_threads: %d, time: %f\n", t, MaxThreads, omp_get_wtime() - startTime);
	#endif
    }
    return 0;
}