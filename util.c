/* 
 * File:   util.c
 * Students : 
 * Fengmin Deng     (dengf, 659332)
 * Jiajie Li        (jiajiel, 631482)
 * Shuangchao Yin   (shuangchaoy, 612511) 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"
#include "paras.h"

PRNGState states[BGQ_THEADS];
BOOL      locks[SIZE+2];

double drand48(void);

void *allocate(int size) {
    void *addr;
    addr = malloc((size_t) size);
    if (addr == NULL) 
        printf("Out of memory!");
    return addr;
}

void initPRNGStates() {
    for(int i = 0; i < BGQ_THEADS; i++) {
        states[i][0] = (unsigned short) (0xFFFF*drand48());
        states[i][1] = (unsigned short) (0xFFFF*drand48());
        states[i][2] = (unsigned short) (0xFFFF*drand48());
    }
}

void initLocks() {
	for (int i = 0; i < SIZE+2; i++) locks[i] = FALSE;
}

void lockForMove(int i) {
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
void unlockForMove(int i) {
	#pragma omp critical (LockRegion)
	{
		locks[i-1] = FALSE;
        locks[i] = FALSE;
        locks[i+1] = FALSE;
	}
}

void lockForPair(int i) {
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
void unlockForPair(int i) {
	#pragma omp critical (LockRegion)
	{
		locks[i-1] = FALSE;
        locks[i] = FALSE;
        locks[i+1] = FALSE;
        locks[i+2] = FALSE;
	}
}

double updateInfectRate(int day) {
    double newInfectRate = INFECT;
        if(day > 1095) { // more than 3 years
            newInfectRate = INFECT / day;
        } else if (day > 730) { // less than 3 years but more than 2
            newInfectRate = INFECT * log(day) / day ;
        } else if (day > 365) { // less than 2 years but more than 1
             newInfectRate = INFECT / log(day);
        }
    return newInfectRate;
}

void printDemographic(int *demographics) {
    char *fileName = (char*) allocate(sizeof(char) * 90);
    char *figuresFileName = (char*) allocate(sizeof(char) * 100);
    char *plotFileName = (char*) allocate(sizeof(char) * 100);
    sprintf(fileName, "%s%d-mz%.2f-zd%.5f-inf%.1f_ChangingIn3Years", 
            DELTA_T, STEPS, MOVE_Z, DEATH_Z, INFECT);
    sprintf(figuresFileName, "%s.txt", fileName);
    sprintf(plotFileName, "%s.png", fileName);
    
    FILE *plotFigures = fopen(figuresFileName, "w+");
    fprintf(plotFigures, "%s,%s,%s,%s\n", "DAY", "FEMALE", "MALE", "ZOMBIE");
    for (int i = 0; i <= STEPS; i++) {
        fprintf(plotFigures, "%d,%d,%d,%d\n", 
                i, demographics[3*i], demographics[3*i+1], demographics[3*i+2]);
    }
    fclose(plotFigures);

    char *pyCall = (char*) allocate(sizeof(char) * 200);
    char *plotTitil = "Chart1. Demographics Zombie Infection Model";
    sprintf(pyCall, "%s %s %s '%s' %s", "python", "plotDemographic.py", 
            figuresFileName, plotTitil, plotFileName);
    system(pyCall);
}
