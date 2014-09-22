/* 
 * File:   datatype.c
 * Author: 
 *
 * Created on September 16, 2014, 09:53 pM
 */

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "paras.h"
#include "datatype.h"

double drand48(void);

char setGender(void) {
    if(drand48() <= PROB_M)
        return 'M';
    else
        return 'F';
}

BOOL canAlive(object obj, double probToDie) {
    return (obj->type == 'Z' && probToDie > DEATH_Z) ||
            (obj->gender == 'M' && probToDie > DEATH_M) ||
                (obj->gender == 'F' && probToDie > DEATH_F);
}

BOOL canReproduce(object obj1, object obj2) {
    if(obj1->type == 'H' && obj2->type == 'H' && obj1->gender != obj2->gender)
        return TRUE;
    else
        return FALSE;
}

char setBabyGender(void) {
    if(drand48() <= BORN_M)
        return 'M';
    else
        return 'F';
}

BOOL canInfect(object obj1, object obj2) {
    if(obj1->type != obj2->type)
        return TRUE;
    else
        return FALSE;
}

void zombiefication(object obj1, object obj2) {
    object human = obj1->type == 'H' ? obj1 : obj2;
    human->type = 'Z';
}