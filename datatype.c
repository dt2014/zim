/* 
 * File:   datatype.c
 * Students : 
 * Fengmin Deng     (dengf, 659332)
 * Jiajie Li        (jiajiel, 631482)
 * Shuangchao Yin   (shuangchaoy, 612511) 
 */

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "paras.h"
#include "datatype.h"


char setGender(double randomNumber) {
    if(randomNumber < PROB_M)
        return 'M';
    else
        return 'F';
}

BOOL isOccupied(object obj) {
    return obj.type != 0;    
}

BOOL isHuman(object obj) {
    return obj.type == 'H';    
}

BOOL isZombie(object obj) {
    return obj.type == 'Z';    
}

BOOL canAlive(object obj, double probToDie) {
    return (obj.type == 'Z' && probToDie > DEATH_Z) ||
            (obj.gender == 'M' && probToDie > DEATH_M) ||
                (obj.gender == 'F' && probToDie > DEATH_F);
}

BOOL canReproduce(object obj1, object obj2) {
    return obj1.type == 'H' && obj2.type == 'H' && obj1.gender != obj2.gender;
}

char setBabyGender(double randomNumber) {
    if(randomNumber < BORN_M)
        return 'M';
    else
        return 'F';
}

BOOL canInfect(object obj1, object obj2) {
    return obj1.type != obj2.type;
}