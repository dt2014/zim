/* 
 * File:   util.c
 * Author: 
 *
 * Created on September 15, 2014, 12:10 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "paras.h"

void *allocate(int size) {
    void *addr;
    addr = malloc((size_t) size);
    if (addr == NULL) 
        printf("Out of memory!");
    return addr;
}


#if defined(_OPENMP)
void lockForMove(int i, BOOL *locks) {
	for (BOOL locked = FALSE; locked == FALSE; /*NOP*/) {
		#pragma omp critical (LockRegion)
		{
			locked = !locks[i-1] && !locks[i] && !locks[i+1];
			if (locked) {
				locks[i-1] = TRUE; locks[i] = TRUE; locks[i+1] = TRUE;
			}
		}
	}
}
void unlockForMove(int i, BOOL *locks) {
	#pragma omp critical (LockRegion)
	{
		locks[i-1] = FALSE; locks[i] = FALSE; locks[i+1] = FALSE;
	}
}

void lockForPair(int i, BOOL *locks) {
	for (BOOL locked = FALSE; locked == FALSE; /*NOP*/) {
		#pragma omp critical (LockRegion)
		{
			locked = !locks[i-1] && !locks[i] && !locks[i+1] && !locks[i+2];
			if (locked) {
				locks[i-1] = TRUE; locks[i] = TRUE; locks[i+1] = TRUE; locks[i+2] = TRUE;
			}
		}
	}
}
void unlockForPair(int i, BOOL *locks) {
	#pragma omp critical (LockRegion)
	{
		locks[i-1] = FALSE; locks[i] = FALSE; locks[i+1] = FALSE; locks[i+2] = FALSE;
	}
}
#endif

int *initDemographic() {
    int *demographic = (int*) allocate(sizeof(int) * (STEPS + 1) * 2);
    for (int i = 0; i <= STEPS*2 + 1; i++) {
        demographic[i] = 0;
    }
    return demographic;
}

void printDemographic(int *demographic) {
    printf("%12s%12s%12s\n", "DAY", "HUMAN", "ZOMBIE");
    for (int i = 0; i <= STEPS; i++) {
        printf("%12d%12d%12d\n", i, demographic[2*i], demographic[2*i+1]);
    }
}