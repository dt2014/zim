/* 
 * File:   util.c
 * Author: 
 *
 * Created on September 15, 2014, 12:10 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include "util.h"

void *allocate(int size) {
    void *addr;
    addr = malloc((size_t) size);
    if (addr == NULL) 
        printf("Out of memory!");
    return addr;
}


#if defined(_OPENMP)
void lock(int i, BOOL *locks) {
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
void unlock(int i, BOOL *locks) {
	#pragma omp critical (LockRegion)
	{
		locks[i-1] = FALSE; locks[i] = FALSE; locks[i+1] = FALSE;
	}
}
#endif

