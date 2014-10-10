/* 
 * File:   mesh.c
 * Students : 
 * Fengmin Deng     (dengf, 659332)
 * Jiajie Li        (jiajiel, 631482)
 * Shuangchao Yin   (shuangchaoy, 612511) 
 */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>
#include "mesh.h"
#include "paras.h"

extern PRNGState states[BGQ_THEADS];

object MESH_A[SIZEI+2][SIZEJ+2];
object MESH_B[SIZEI+2][SIZEJ+2];
object (*MeshA)[SIZEJ+2] = MESH_A;
object (*MeshB)[SIZEJ+2] = MESH_B;
int    demographics[DMGP_CURVES*(STEPS+1)];

double  erand48(unsigned short xsubi[3]);
double  drand48(void);

void initMesh() {
    #pragma omp parallel for default(none) shared(MeshA, MeshB)
	for (int i = 0; i < SIZEI+2; i++) {
		for (int j = 0; j < SIZEJ+2; j++) {
            MeshA[i][j].type = 0;
            MeshA[i][j].gender = 0;
            MeshA[i][j].age = 0;
            MeshB[i][j].type = 0;
            MeshB[i][j].gender = 0;
            MeshB[i][j].age = 0;
        }
	}
}

void putHumanOnMesh() {
    #pragma omp parallel for default(none) shared(MeshA, states)
	for (int i = 1; i <= SIZEI; i++) {
		for (int j = 1; j <= SIZEJ; j++) {
			if (erand48(states[omp_get_thread_num()]) < POP_DENSITY) {
				MeshA[i][j].type = 'H';
                MeshA[i][j].gender = 
                        setGender(erand48(states[omp_get_thread_num()]));				
				MeshA[i][j].age =
                        (char) (erand48(states[omp_get_thread_num()]) * 100);
			}
		}
	}
}

void putZombieOnMesh() {
    for (int i = 0; i < INIT_Z_QTY; i++ ) {
        int cell_i = (int) (drand48() * SIZEI);
        int cell_j = (int) (drand48() * SIZEJ);
        MeshA[cell_i][cell_j].type = 'Z';
        MeshA[cell_i][cell_j].gender = setGender(drand48());
        MeshA[cell_i][cell_j].age = (char) (drand48() * 100);
    }
}

BOOL canMove(object obj, double move, double factor) {
    return (isHuman(obj) && move < factor * MOVE_H) ||
            (isZombie(obj) && move < factor * MOVE_Z);
}

void removeFromMeshAToMeshB(int Ai, int Aj, int Bi, int Bj) {
    MeshB[Bi][Bj].type = MeshA[Ai][Aj].type;
    MeshB[Bi][Bj].gender = MeshA[Ai][Aj].gender;
    MeshB[Bi][Bj].age = MeshA[Ai][Aj].age;
    MeshA[Ai][Aj].type = 0;
}
/* The object is moved MeshB in any case.
 * Regarding the position to move, it depends on the probability to move and
 * the occupation of the other positions around.
 */
void moveObject(double move, int i, int j){
    if(canMove(MeshA[i][j], move, 1.0) && 
            (!isOccupied(MeshA[i-1][j])) && (!isOccupied(MeshB[i-1][j]))) {
        removeFromMeshAToMeshB(i, j, i-1, j);
    } else if (canMove(MeshA[i][j], move, 2.0) && 
            (!isOccupied(MeshA[i+1][j])) && (!isOccupied(MeshB[i+1][j]))) {
        removeFromMeshAToMeshB(i, j, i+1, j);
    } else if (canMove(MeshA[i][j], move, 3.0) &&
            (!isOccupied(MeshA[i][j-1])) && (!isOccupied(MeshB[i][j-1]))) {
        removeFromMeshAToMeshB(i, j, i, j-1);
    } else if (canMove(MeshA[i][j], move, 4.0) &&
            (!isOccupied(MeshA[i][j+1])) && (!isOccupied(MeshB[i][j+1]))) {
        removeFromMeshAToMeshB(i, j, i, j+1);
    } else {
        removeFromMeshAToMeshB(i, j, i, j);
    }
}

void moveInMeshB(int oldi, int oldj, int newi, int newj) {
    MeshB[newi][newj].type = MeshB[oldi][oldj].type;
    MeshB[newi][newj].gender = MeshB[oldi][oldj].gender;
    MeshB[newi][newj].age = MeshB[oldi][oldj].age;
    MeshB[oldi][oldj].type = 0;
}

void ScanOutOfRange(int rank) {
    
	for(int i = 1; i <= SIZEI; i++) {
		if (rank == 1&&isOccupied(MeshB[i][0])) moveInMeshB(i, 0, i, 1);
		if (rank == 0&&isOccupied(MeshB[i][SIZEJ+1])) moveInMeshB(i, SIZEJ+1, i, SIZEJ);
	}
    
	for(int j = 1; j <= SIZEJ; j++) {
		if(isOccupied(MeshB[0][j])) moveInMeshB(0, j, 1, j);
		if(isOccupied(MeshB[SIZEI+1][j])) moveInMeshB(SIZEI+1, j, SIZEI, j);
	}
}

void swap(object (**MeshA)[], object (**MeshB)[]){
    object (*tmp)[SIZEJ+2] = *MeshA;  
    *MeshA = *MeshB;  
    *MeshB = tmp;  
}

void putBabyOnMesh(int i, int j) {
    MeshB[i][j].type = 'H';
    MeshB[i][j].gender = setBabyGender(erand48(states[omp_get_thread_num()]));
    MeshB[i][j].age = 0;
}

/* If not fulfiling reproduction conditions, just remove from MeshA to MeshB */
void tryToReproduce(int i, int j){
    if (isOccupied(MeshA[i+1][j]) && canReproduce(MeshA[i][j], MeshA[i+1][j])){
        removeFromMeshAToMeshB(i+1, j, i+1, j);
        if ((!isOccupied(MeshA[i-1][j])) && (!isOccupied(MeshB[i-1][j]))) {
            putBabyOnMesh(i-1, j);
        }else if((!isOccupied(MeshA[i][j-1])) && (!isOccupied(MeshB[i][j-1]))){
            putBabyOnMesh(i, j-1);
        }else if((!isOccupied(MeshA[i+1][j-1]))&&(!isOccupied(MeshB[i+1][j-1]))){
            putBabyOnMesh(i+1, j-1);
        }else if((!isOccupied(MeshA[i][j+1]))&&(!isOccupied(MeshB[i][j+1]))){
            putBabyOnMesh(i, j+1);
        }else if((!isOccupied(MeshA[i+1][j+1]))&&(!isOccupied(MeshB[i+1][j+1]))){
            putBabyOnMesh(i+1, j+1);
        }else if((!isOccupied(MeshA[i+2][j]))&&(!isOccupied(MeshB[i+2][j]))){
            putBabyOnMesh(i+2, j);
        }
    } else if (isOccupied(MeshA[i][j+1]) &&
            canReproduce(MeshA[i][j], MeshA[i][j+1])) {
        removeFromMeshAToMeshB(i, j+1, i, j+1);
        if ((!isOccupied(MeshA[i][j-1]))&&(!isOccupied(MeshB[i][j-1]))){
            putBabyOnMesh(i, j-1);
        }else if((!isOccupied(MeshA[i-1][j]))&&(!isOccupied(MeshB[i-1][j]))){
            putBabyOnMesh(i-1, j);
        }else if((!isOccupied(MeshA[i-1][j+1]))&&(!isOccupied(MeshB[i-1][j+1]))){
            putBabyOnMesh(i-1, j+1);
        }else if((!isOccupied(MeshA[i+1][j]))&&(!isOccupied(MeshB[i+1][j]))){
            putBabyOnMesh(i+1, j);
        }else if((!isOccupied(MeshA[i+1][j+1]))&&(!isOccupied(MeshB[i+1][j+1]))){
            putBabyOnMesh(i+1, j+1);
        }else if((!isOccupied(MeshA[i][j+2]))&&(!isOccupied(MeshB[i][j+2]))){
            putBabyOnMesh(i, j+2);
        }
    } else {
        removeFromMeshAToMeshB(i, j, i, j);
    }
}

/* If not fulfiling zombification conditions, just remove from MeshA to MeshB */
void tryToInfect(int i, int j) {
    if (isOccupied(MeshA[i+1][j]) && canInfect(MeshA[i][j], MeshA[i+1][j])) {
        removeFromMeshAToMeshB(i, j, i, j);
        removeFromMeshAToMeshB(i+1, j, i+1, j);
        MeshB[i][j].type = 'Z';
        MeshB[i+1][j].type = 'Z';
    }else if(isOccupied(MeshA[i][j+1]) && canInfect(MeshA[i][j],MeshA[i][j+1])){
        removeFromMeshAToMeshB(i, j, i, j);
        removeFromMeshAToMeshB(i, j+1, i, j+1);
        MeshB[i][j].type = 'Z';
        MeshB[i][j+1].type = 'Z';
    } else {
        removeFromMeshAToMeshB(i, j, i, j);
    }
}

void addDemographicNbr(int t) {
	int fQty = 0;
    int mQty = 0;
    int zQty = 0;
	#pragma omp parallel for default(none) shared(MeshA) \
            reduction(+:fQty,mQty,zQty)
	for (int i = 1; i <= SIZEI; i++) {
		for (int j = 1; j <= SIZEJ; j++) {
			if (isOccupied(MeshA[i][j])) {
                if (MeshA[i][j].type == 'Z') zQty++;
                else if (MeshA[i][j].gender == 'F') fQty++;
                else mQty++;
            }
		} 
	}
    demographics[3*t] = fQty;
    demographics[3*t+1] = mQty;
    demographics[3*t+2] = zQty;
}

void printMesh(int day) {
    FILE *printmesh = fopen("printmesh.txt", "a+");
    fprintf(printmesh, "Day %d:\n", day);
	for (int i = 0; i <= SIZEI+1; i++) {
        for (int j = 0; j <= SIZEJ+1; j++) fprintf(printmesh, "----");
        fprintf(printmesh, "\n");
		for (int j = 0; j <= SIZEJ+1; j++) {
			if (isOccupied(MeshA[i][j])) {
				if(MeshA[i][j].age < 10)
                    fprintf(printmesh, "|%c0%d", MeshA[i][j].type == 'H' ? 
                        MeshA[i][j].gender : 'Z', MeshA[i][j].age);
				else
                    fprintf(printmesh, "|%c%d", MeshA[i][j].type == 'H' ? 
                        MeshA[i][j].gender : 'Z', MeshA[i][j].age);
			}
			else
                fprintf(printmesh, "|   ");
		}
		fprintf(printmesh, "|\n");
	}
    for (int i = 0; i <= SIZEI+1; i++) fprintf(printmesh, "----");
	fprintf(printmesh, "\n\n");
    fclose(printmesh);
}