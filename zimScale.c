/* 
 * File:   zimScale.c => This main file is for testing the strong scaling.
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
extern BOOL      locks[SIZEI + 2];
extern object    (*MeshA)[SIZEJ+2];
extern object    (*MeshB)[SIZEJ+2];
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
    initPRNGStates();
	initLocks();
    max_threads = omp_get_max_threads();
    
    for (int t = 1; t <= max_threads; t *= 2) {//start of the scaling test loop
    double startTime = omp_get_wtime();
    omp_set_num_threads(t); //constrain num_threads for next parallelization
    initMesh();
    omp_set_num_threads(t);
    putHumanOnMesh(); /* parallelized */
    putZombieOnMesh();
    omp_set_num_threads(t); 
    addDemographicNbr(0); /* parallelized */
	//printMesh(0);
    
	for (int n = 0; n < STEPS; n++) {
        #pragma omp parallel default(none) \
                shared(MeshA, MeshB, locks, n, states) num_threads(t)
        { /* start of parallel region within time loop */
            
        /* 1. MOVE */
        #pragma omp for
		for (int i = 1; i <= SIZEI; i++) {
			lockForMove(i);
			for (int j = 1; j <= SIZEJ; j++) 
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
        for (int i = 1; i <= SIZEI; i++) {
			for (int j = 1; j <= SIZEJ; j++) {
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
		for (int i = 1; i < SIZEI; i++) {
			lockForPair(i);
			for (int j = 1; j < SIZEJ; j++) {
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
        double updatedINFECT = n < 365 ? INFECT : 0.001;
        #pragma omp for
		for (int i = 1; i < SIZEI; i++) {
			lockForPair(i);
			for (int j = 1; j < SIZEJ; j++) 
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
        omp_set_num_threads(t); 
        addDemographicNbr(n+1); /* parallelized */
        //printMesh(n);
	}
    //printMesh(STEPS);
    //printDemographic(demographics);
    printf("WorkingThreads: %d, MaxThreads: %d, time: %f\n",
           t, max_threads, omp_get_wtime() - startTime);
    }// end of the scaling test loop
    return 0;
}