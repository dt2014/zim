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
#include "mesh.h"
#include "paras.h"

extern PRNGState states[BGQ_THEADS];
extern BOOL      locks[SIZE + 2];
extern object    (*MeshA)[SIZE+2];
extern object    (*MeshB)[SIZE+2];
extern int       demographics[DMGP_CURVES*(STEPS+1)];

int     max_threads     = 1;
long    seed            = 8767134;

void    srand48(long int seedval);
double  drand48(void);
double  erand48(unsigned short xsubi[3]);

int main(int argc, char** argv) {
    if(argc > 1) {
        seed = atol(argv[1]);
    }
    srand48(seed);
    
    double startTime = omp_get_wtime();
    max_threads = omp_get_max_threads();
    initPRNGStates();
	initLocks();
    putHumanOnMesh(); /* parallelized */
    putZombieOnMesh();
    addDemographicNbr(0); /* parallelized */
	//printMesh(0);
    
	for (int n = 0; n < STEPS; n++) {
        
        #pragma omp parallel default(none) \
                shared(MeshA, MeshB, locks, n, states)
        { /* start of parallel region within time loop */
            
        /* 1. MOVE */
        #pragma omp for
		for (int i = 1; i <= SIZE; i++) {
			lockForMove(i);
			for (int j = 1; j <= SIZE; j++) 
                if (isOccupied(MeshA[i][j])) {
                    double move = erand48(states[omp_get_thread_num()]);
                    moveObject(move, i, j);
                } 
			unlockForMove(i);
		}
        /* 2. BOUNDARY CHECK AFTER MOVE */
		ScanOutOfRange();
        #pragma omp single
        swap(&MeshA, &MeshB);
        /* 3. DEATH */
        #pragma omp for
        for (int i = 1; i <= SIZE; i++) {
			for (int j = 1; j <= SIZE; j++) {
                if (isOccupied(MeshA[i][j])) { 
                    double death = erand48(states[omp_get_thread_num()]);
                    if (canAlive(MeshA[i][j], death)) {
                        removeFromMeshAToMeshB(i, j, i, j);
                    } else { //otherwise dies or decomposes
                        MeshA[i][j].type = 0;
                    }
                }
            }
		}
        #pragma omp single
        swap(&MeshA, &MeshB);
        
        /* 4. BIRTH */
        #pragma omp for
		for (int i = 1; i < SIZE; i++) {
			lockForPair(i);
			for (int j = 1; j < SIZE; j++) {
                if (isOccupied(MeshA[i][j])) {
                    double birth = erand48(states[omp_get_thread_num()]);
                    if (birth < BIRTH) {
                        tryToReproduce(i, j);
                    } else {
                        removeFromMeshAToMeshB(i, j, i, j);
                    }
                }
            }
			unlockForPair(i);
		}
        #pragma omp single
		swap(&MeshA, &MeshB);
        
        /* 5. ZOMBIEFICATION */
        double updatedINFECT = updateInfectRate(n);
        #pragma omp for
		for (int i = 1; i < SIZE; i++) {
			lockForPair(i);
			for (int j = 1; j < SIZE; j++) 
                if (isOccupied(MeshA[i][j])) {                    
                    double infect = erand48(states[omp_get_thread_num()]);
                    if (infect < updatedINFECT) {
                        tryToInfect(i, j);
                    } else {
                        removeFromMeshAToMeshB(i, j, i, j);
                    }
                }
			unlockForPair(i);
		}
        #pragma omp single
		swap(&MeshA, &MeshB);
        } /* bracket for the end of parallel region within time loop */
        addDemographicNbr(n+1); /* parallelized */
        //printMesh(n);
	}
    //printMesh(STEPS);
    printDemographic(demographics);
    printf("Max_threads: %d, time: %f\n\n", max_threads, omp_get_wtime() - startTime);
    return 0;
}