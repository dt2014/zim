/* 
 * File:   zim.c
 * Students : 
 * Fengmin Deng     (dengf, 659332)
 * Jiajie Li        (jiajiel, 631482)
 * Shuangchao Yin   (shuangchaoy, 612511) 
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "mesh.h"
#include "paras.h"

int max_threads = 1;

//int demographics[DMGP_CURVES*(SIZE+1)];

void    srand48(long int seedval);
double  drand48(void);
double  erand48(unsigned short xsubi[3]);

int main(int argc, char** argv) {
    #if defined(_OPENMP) 
    double startTime = omp_get_wtime();
    max_threads = omp_get_max_threads();
	#endif

    srand48(86585887);
    
    PRNGState *states = initPRNGStates(max_threads);
	BOOL *locks = initLocks();
	object **MeshA = CreateMesh(SIZE + 2, SIZE + 2);
	object **MeshB = CreateMesh (SIZE + 2, SIZE + 2);
    
    putHumanOnMesh(MeshA, states); //parallelized
    
    // put zombies on mesh
    for (int i = 0; i < INIT_Z_QTY; i++ ) {
        object zombie = (object) allocate(sizeof(struct Object));
        zombie->gender = setGender(drand48());				
		zombie->age = rand()%99+1;
		zombie->type = 'Z';
        int cell_i = (int) (drand48() * SIZE);
        int cell_j = (int) (drand48() * SIZE);
        MeshA[cell_i][cell_j] = zombie;
    }
    
    int *demographics = initDemographic();
    addDemographicNbr(demographics, MeshA, 0);//parallelized
	//printMesh(MeshA);
    
	for (int n = 0; n < STEPS; n++) {
        #pragma omp parallel default(none) shared(MeshA, MeshB, locks, n, states)
        { // start of parallel region within time loop
        /* 1. MOVE */
        #pragma omp for
		for (int i = 1; i <= SIZE; i++) {
			#if defined(_OPENMP)
			lockForMove(i, locks);
			#endif
			for (int j = 1; j <= SIZE; j++) 
                if (MeshA[i][j] != NULL) {
                    double move = erand48(states[omp_get_thread_num()]);
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
		ScanOutOfRange(MeshB);
        #pragma omp single
        swap(&MeshA, &MeshB);
        
        /* 3. DEATH */
        #pragma omp for
        for (int i = 1; i <= SIZE; i++) {
			for (int j = 1; j <= SIZE; j++) 
                if (MeshA[i][j] != NULL) { 
                    double death = erand48(states[omp_get_thread_num()]);
                    if (canAlive(MeshA[i][j], death)) {
                        MeshB[i][j] = MeshA[i][j];
                    } else {
                        free(MeshA[i][j]);
                    }
                    MeshA[i][j] = NULL;
                }
		}
        #pragma omp single
        swap(&MeshA, &MeshB);
        
        /* 4. BIRTH */
        #pragma omp for
		for (int i = 1; i < SIZE; i++) {
            #if defined(_OPENMP)
			lockForPair(i, locks);
			#endif
			for (int j = 1; j < SIZE; j++) 
                if (MeshA[i][j] != NULL) {
                    double birth = erand48(states[omp_get_thread_num()]);
                    if (birth < BIRTH && MeshA[i+1][j] != NULL && 
                            canReproduce(MeshA[i][j], MeshA[i+1][j])) {
                        MeshB[i+1][j] = MeshA[i+1][j];
                        MeshA[i+1][j] = NULL;
                        
                        object baby = (object) allocate(sizeof(struct Object));
                        baby->gender = setBabyGender(erand48(states[omp_get_thread_num()]));
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
                            //free(baby);
                        }
                        
                    } else if (birth < BIRTH && MeshA[i][j+1] != NULL && 
                            canReproduce(MeshA[i][j], MeshA[i][j+1])) {
                        MeshB[i][j+1] = MeshA[i][j+1];
                        MeshA[i][j+1] = NULL;
                        
                        object baby = (object) allocate(sizeof(struct Object));
                        baby->gender = setBabyGender(erand48(states[omp_get_thread_num()]));
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
                            //free(baby);
                        }
                    }
                    MeshB[i][j] = MeshA[i][j];
                    MeshA[i][j] = NULL;
                }
            #if defined(_OPENMP)
			unlockForPair(i, locks);
			#endif
		}
        #pragma omp single
		swap(&MeshA, &MeshB);
        
        /* 5. ZOMBIEFICATION */
        double updatedProbOfInfect = n < 365 ? INFECT_EARLY / (1 + log(1 + n)) : INFECT_LATER / (1 + log(1 + n));
        #pragma omp for
		for (int i = 1; i < SIZE; i++) {
            #if defined(_OPENMP)
			lockForPair(i, locks);
			#endif
			for (int j = 1; j < SIZE; j++) 
                if (MeshA[i][j] != NULL) {                    
                    double infect = erand48(states[omp_get_thread_num()]);
                    
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
        #pragma omp single
		swap(&MeshA, &MeshB);
        } // bracket for the parallel region within time loop
        addDemographicNbr(demographics, MeshA, n+1);
	}
    //printMesh(MeshA);
    printDemographic(demographics);
    #if defined(_OPENMP)
    printf("Max_threads: %d, time: %f\n\n", max_threads, omp_get_wtime() - startTime);
	#endif
    return 0;
}