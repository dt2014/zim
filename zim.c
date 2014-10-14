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
#include <mpi.h>
#include "mesh.h"
#include "paras.h"
#include "MPI_Activity.h"

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
    
	int rank, size;
	//object temp[1024];
	MPI_Init (&argc, &argv);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	
	MPI_Datatype cellDatatype;
	MPI_Type_contiguous(sizeof(object), MPI_BYTE, &cellDatatype);
	MPI_Type_commit (&cellDatatype);

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

		/* exchange the boundry condition between two MPI node */
		exchangeBoundryCondition(rank,MeshA,cellDatatype);
	
        #pragma omp parallel default(none) \
                shared(MeshA, MeshB, locks, n, states)
        { 
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
		}
			
        /* 2. BOUNDARY CHECK AFTER MOVE */
		ScanOutOfRange(rank);
       	
		/* transfer the object that moved to the ghost cell */	
		transferBoundry(rank,MeshB,cellDatatype);
		clearOutGhostcell(rank,MeshA,cellDatatype);	
  	    swap(&MeshA, &MeshB);
        /* 3. DEATH */
		 #pragma omp parallel default(none) \
             shared(MeshA, MeshB, locks, n, states)
        {
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
		}
		clearOutGhostcell(rank,MeshA,cellDatatype);
        swap(&MeshA, &MeshB);
        
		/* exchange the boundry condition between two MPI node */
		exchangeBoundryCondition(rank,MeshA,cellDatatype);
		
        /* 4. BIRTH */
		#pragma omp parallel default(none) \
             shared(MeshA, MeshB, locks, n, states)
        {
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
		}
        
		/* transfer the object that moved to the ghost cell */	
		transferBoundry(rank,MeshB,cellDatatype);
		clearOutGhostcell(rank,MeshA,cellDatatype);
		swap(&MeshA, &MeshB);
        
		/* exchange the boundry condition between two MPI node */
		exchangeBoundryCondition(rank,MeshA,cellDatatype);
		
        /* 5. ZOMBIEFICATION */
        //double updatedINFECT = updateInfectRate(n);
		double updatedINFECT = INFECT;
        #pragma omp parallel default(none) \
             shared(MeshA, MeshB, locks, n, states,updatedINFECT)
        {
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
		}
       
	   	/* transfer the object that moved to the ghost cell */	
		transferBoundry(rank,MeshB,cellDatatype);
		clearOutGhostcell(rank,MeshA,cellDatatype);
		swap(&MeshA, &MeshB);
		
        /* bracket for the end of parallel region within time loop */
        addDemographicNbr(n+1); /* parallelized */
        //printMesh(n);
	}
    //printMesh(STEPS);
    if(rank == 0)
	{
		MPI_Send(&demographics, DMGP_CURVES*(STEPS+1), MPI_INT, 1, TAG, MPI_COMM_WORLD);
	}
	if(rank == 1)
	{
		int demographicsOfRank1[DMGP_CURVES*(STEPS+1)];
		MPI_Status status;
		MPI_Recv(&demographicsOfRank1, DMGP_CURVES*(STEPS+1), MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
		for(int i = 0; i<DMGP_CURVES*(STEPS+1);i++)
			demographics[i] = demographics[i] + demographicsOfRank1[i];
		printDemographic(demographics);
	}
    printf("Max_threads: %d, time: %f\n\n", max_threads, omp_get_wtime() - startTime);
	MPI_Finalize();
	return 0;
}





