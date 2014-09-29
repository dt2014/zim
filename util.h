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

void    *allocate(int size);
void    initPRNGStates();
void    initLocks();
void    lockForMove(int i);
void    unlockForMove(int i);
void    lockForPair(int i);
void    unlockForPair(int i);
double  updateInfectRate(int day);
void    printDemographic(int *demographics);

#endif	/* UTIL_H */

