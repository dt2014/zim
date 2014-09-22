/* 
 * File:   mesh.h
 * Students : 
 * Fengmin Deng     (dengf, 659332)
 * Jiajie Li        (jiajiel, 631482)
 * Shuangchao Yin   (shuangchaoy, 612511) 
 */

#ifndef MESH_H
#define	MESH_H

#include "datatype.h"

object **CreateMesh(int I, int J);
object **ScanOutOfRange(object **Mesh);
void swap(object*** a, object*** b);
void addDemographicNbr(int *demographic, object **Mesh, int t);
void printMesh(object **Mesh);

#endif	/* MESH_H */

