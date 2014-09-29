/* 
 * File:   datatype.h
 * Students : 
 * Fengmin Deng     (dengf, 659332)
 * Jiajie Li        (jiajiel, 631482)
 * Shuangchao Yin   (shuangchaoy, 612511) 
 */

#ifndef DATATYPE_H
#define	DATATYPE_H

#include "util.h"

typedef struct{
    char type;   /* 'H' for human, 'Z' for zombie, 0 for empty cell */
    char gender; /* 'M' for male, 'F' for female */
	char age;	 /* from 0 to 100 */
	char pace;   /* not used yet */
} object;

char setGender(double randomNumber);
BOOL isOccupied(object obj);
BOOL isHuman(object obj);
BOOL isZombie(object obj);
BOOL canAlive(object obj, double probToDie);
BOOL canReproduce(object bj1, object obj2);
char setBabyGender(double randomNumber);
BOOL canInfect(object obj1, object obj2);

#endif	/* DATATYPE_H */

