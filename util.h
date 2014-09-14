/* 
 * File:   util.h
 * Author: 
 *
 * Created on September 15, 2014, 12:10 AM
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

void *allocate(int size);
void lock(int i, BOOL *locks);
void unlock(int i, BOOL *locks);

#endif	/* UTIL_H */

