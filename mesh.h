/* 
 * File:   mesh.h
 * Author: 
 *
 * Created on September 15, 2014, 12:52 AM
 */

#ifndef MESH_H
#define	MESH_H

#include "datatype.h"

Spot **CreateMesh(int I, int J);
Spot **ScanOutOfRange(Spot **Mesh);
void swap(Spot*** a, Spot*** b);

void printMeshHeading(Spot **Mesh, int t);
void printMesh(Spot **Mesh);

#endif	/* MESH_H */

