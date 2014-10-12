#ifndef MPI_Activity_H
#define	MPI_Activity_H

#include <mpi.h>
#include "paras.h"
#include "mesh.h"
#define TAG 1

void exchangeBoundryCondition(int rank,object (*mesh)[SIZEJ+2],MPI_Datatype cellDatatype);
void transferBoundry(int rank,object (*mesh)[SIZEJ+2],MPI_Datatype cellDatatype);

#endif	/* MPI_Activity_H */
