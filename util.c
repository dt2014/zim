/* 
 * File:   util.c
 * Students : 
 * Fengmin Deng     (dengf, 659332)
 * Jiajie Li        (jiajiel, 631482)
 * Shuangchao Yin   (shuangchaoy, 612511) 
 */

#include <stdio.h>
#include <stdlib.h>
//#include <Python.h>
#include "util.h"
#include "paras.h"

double drand48(void);

void *allocate(int size) {
    void *addr;
    addr = malloc((size_t) size);
    if (addr == NULL) 
        printf("Out of memory!");
    return addr;
}

PRNGState *initPRNGStates(int max_threads){
    PRNGState *states = (PRNGState*) allocate(sizeof(PRNGState) * max_threads);
    for(int i = 0; i < max_threads; i++) {
        states[i][0] = (unsigned short) (0xFFFF*drand48());
        states[i][1] = (unsigned short) (0xFFFF*drand48());
        states[i][2] = (unsigned short) (0xFFFF*drand48());
    }
    return states;
}

BOOL *initLocks() {
    BOOL *locks = (BOOL*) allocate((SIZE + 2) * sizeof(BOOL*));
	for (int i = 0; i < SIZE + 2; i++)
        locks[i] = FALSE;
    return locks;
}

#if defined(_OPENMP)
void lockForMove(int i, BOOL *locks) {
	for (BOOL locked = FALSE; locked == FALSE; /*NOP*/) {
		#pragma omp critical (LockRegion)
		{
			locked = !locks[i-1] && !locks[i] && !locks[i+1];
			if (locked) {
				locks[i-1] = TRUE;
                locks[i] = TRUE;
                locks[i+1] = TRUE;
			}
		}
	}
}
void unlockForMove(int i, BOOL *locks) {
	#pragma omp critical (LockRegion)
	{
		locks[i-1] = FALSE;
        locks[i] = FALSE;
        locks[i+1] = FALSE;
	}
}

void lockForPair(int i, BOOL *locks) {
	for (BOOL locked = FALSE; locked == FALSE; /*NOP*/) {
		#pragma omp critical (LockRegion)
		{
			locked = !locks[i-1] && !locks[i] && !locks[i+1] && !locks[i+2];
			if (locked) {
				locks[i-1] = TRUE;
                locks[i] = TRUE;
                locks[i+1] = TRUE;
                locks[i+2] = TRUE;
			}
		}
	}
}
void unlockForPair(int i, BOOL *locks) {
	#pragma omp critical (LockRegion)
	{
		locks[i-1] = FALSE;
        locks[i] = FALSE;
        locks[i+1] = FALSE;
        locks[i+2] = FALSE;
	}
}
#endif

int *initDemographic() {
    int demographicSize = (STEPS + 1) * DMGP_CURVES;
    int *demographics = (int*) allocate(demographicSize * sizeof(int));
    for (int i = 0; i < demographicSize; i++) {
        demographics[i] = 0;
    }
    return demographics;
}

/*
void plotDemographic(char *figuresFile, char *plotTitle, char *pngName) {
    char *argv[4];
    argv[0] = "plotDemographic.py";
    argv[1] = figuresFile;
    argv[2] = plotTitle;
    argv[3] = pngName;
    FILE *pyScript = fopen("plotDemographic.py", "r");
    Py_Initialize();
    PySys_SetArgv(4, argv);
    if(PyRun_SimpleFile(pyScript, "plotDemographic.py") != 0) {
        fclose(pyScript);
        sprintf(stderr, "PyRun_SimpleFile(%s) failed!", "plotDemographic.py");
    }
    fclose(pyScript);
    Py_Finalize();
}*/

void printDemographic(int *demographic) {
    char *fileName = (char*) allocate(sizeof(char) * 90);
    char *figuresFileName = (char*) allocate(sizeof(char) * 100);
    char *plotFileName = (char*) allocate(sizeof(char) * 100);
    sprintf(fileName, "%s%d-%.5f-zd%.5f-infLOG%.2fToLOG%.4f", 
            DELTA_T, STEPS, BIRTH, DEATH_Z, INFECT_EARLY, INFECT_LATER);
    sprintf(figuresFileName, "%s.txt", fileName);
    sprintf(plotFileName, "%s.png", fileName);
    
    FILE *plotFigures = fopen(figuresFileName, "w+");
    fprintf(plotFigures, "%s,%s,%s,%s\n", "DAY", "FEMALE", "MALE", "ZOMBIE");
    for (int i = 0; i <= STEPS; i++) {
        fprintf(plotFigures, "%d,%d,%d,%d\n", 
                i, demographic[3*i], demographic[3*i+1], demographic[3*i+2]);
    }
    fclose(plotFigures);
    
//plotDemographic(figuresFileName, "Demographic Human & Zombie", plotFileName);
    char *pyCall = (char*) allocate(sizeof(char) * 200);
    char *plotTitil = "Demographics Zombie Infection Model";
    sprintf(pyCall, "%s %s %s '%s' %s", "python", "plotDemographic.py", 
            figuresFileName, plotTitil, plotFileName);
    system(pyCall);
}

