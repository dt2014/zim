/* 
 * File:   datatype.h
 * Author: 
 *
 * Created on September 15, 2014, 12:33 AM
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

char setGender(void);
BOOL canReproduce(object bj1, object obj2);

#endif	/* DATATYPE_H */

