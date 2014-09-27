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

typedef struct Object *object;

struct Object{
	int age;	
	char gender; //M/F
	char type;//zombie or human Z/H 
};

char setGender(double randomNumber);
BOOL canAlive(object obj, double probToDie);
BOOL canReproduce(object bj1, object obj2);
char setBabyGender(double randomNumber);
BOOL canInfect(object obj1, object obj2);
void zombiefication(object obj1, object obj2);

#endif	/* DATATYPE_H */

