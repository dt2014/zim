/* 
 * File:   datatype.c
 * Author: 
 *
 * Created on September 16, 2014, 09:53 pM
 */

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "datatype.h"

char getGender(void) {
    if(rand()%10 >= 5)
        return 'F';
    else
        return 'M';
}

BOOL canReproduce(Object *obj1, Object *obj2) {
    if(obj1->type == 'H' && obj2->type == 'H' && obj1->gender != obj2->gender)
        return TRUE;
    else
        return FALSE;
}
