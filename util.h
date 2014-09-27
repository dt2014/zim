/* 
 * File:   util.h
 * Students : 
 * Fengmin Deng     (dengf, 659332)
 * Jiajie Li        (jiajiel, 631482)
 * Shuangchao Yin   (shuangchaoy, 612511) 
 */

#ifndef UTIL_H
#define	UTIL_H

#ifndef FALSE
#define FALSE ((BOOL) 0)
#endif
#ifndef TRUE
#define TRUE  ((BOOL) 1)
#endif
#ifndef BOOL
#define BOOL    char
#endif

typedef unsigned short PRNGState[3];

void        *allocate(int size);
PRNGState   *initPRNGStates(int max_threads);
BOOL        *initLocks();
void        lockForMove(int i, BOOL *locks);
void        unlockForMove(int i, BOOL *locks);
void        lockForPair(int i, BOOL *locks);
void        unlockForPair(int i, BOOL *locks);
int         *initDemographic();
void        printDemographic(int *demographic);

#endif	/* UTIL_H */

