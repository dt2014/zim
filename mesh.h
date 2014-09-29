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

void initMesh();
void putHumanOnMesh();
void putZombieOnMesh();
void removeFromMeshAToMeshB(int Ai, int Aj, int Bi, int Bj);
void moveObject(double move, int i, int j);
void ScanOutOfRange();
void swap(object (**MeshA)[], object (**MeshB)[]);
void tryToReproduce(int i, int j);
void tryToInfect(int i, int j);
void addDemographicNbr(int t);
void printMesh(int day);

#endif	/* MESH_H */

