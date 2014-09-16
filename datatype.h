/* 
 * File:   datatype.h
 * Author: 
 *
 * Created on September 15, 2014, 12:33 AM
 */

#ifndef DATATYPE_H
#define	DATATYPE_H

#include "util.h"

typedef struct {
	int age;	
	char gender; //M/F
	char type;//zombie or human Z/H 
} Object;

typedef struct{
	BOOL 		taken;
	Object		*object;
} Spot;

char getGender(void);
BOOL canReproduce(Object *obj1, Object *obj2);

#endif	/* DATATYPE_H */

