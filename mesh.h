/* 
 * File:   mesh.h
 * Author: 
 *
 * Created on September 15, 2014, 12:52 AM
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

