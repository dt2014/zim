#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "MPI_Activity.h"
void exchangeBoundryCondition(int rank,object (*mesh)[SIZEJ+2],MPI_Datatype cellDatatype)
{
	object temp[1026];
	MPI_Status status;
	if(rank == 0) //rank 0 sent first and receive latter
	{
		for(int i =0; i<1026; i++) //prepare for the line to sent
		{
			temp[i].type = mesh[i][1].type;
			temp[i].gender = mesh[i][1].gender;
			temp[i].age = mesh[i][1].age;
		}
		MPI_Send(&temp, 1026, cellDatatype, 1, TAG, MPI_COMM_WORLD);
		
		MPI_Recv(&temp, 1026, cellDatatype, 1, TAG, MPI_COMM_WORLD, &status);
		for(int i =0; i < 1026; i++)
		{
			mesh[i][0].type = temp[i].type;
			mesh[i][0].gender = temp[i].gender;
			mesh[i][0].age = temp[i].age;
		}
	}
	if(rank == 1) //rank 1 receive first and send latter
	{
		MPI_Recv(&temp, 1026, cellDatatype, 0, TAG, MPI_COMM_WORLD, &status);
		for(int i =0; i < 1026; i++)
		{
			mesh[i][SIZEJ+1].type = temp[i].type;
			mesh[i][SIZEJ+1].gender = temp[i].gender;
			mesh[i][SIZEJ+1].age = temp[i].age;
		}
		
		for(int i =0; i<1026; i++) //prepare for the line to sent
		{
			temp[i].type = mesh[i][SIZEJ].type;
			temp[i].gender = mesh[i][SIZEJ].gender;
			temp[i].age = mesh[i][SIZEJ].age;
		}
		MPI_Send(&temp, 1026, cellDatatype, 0, TAG, MPI_COMM_WORLD);
	}
}

void transferBoundry(int rank,object (*mesh)[SIZEJ+2],MPI_Datatype cellDatatype)
{
	object temp[1026];
	MPI_Status status;
	if(rank == 0)
	{   
		//get the ghost cell
		for(int i =0; i<1026; i++)  
		{
			temp[i].type = mesh[i][0].type;
			temp[i].gender = mesh[i][0].gender;
			temp[i].age = mesh[i][0].age;
		}
		//send the ghost cell to rank 1 
		MPI_Send(&temp, 1026, cellDatatype, 1, TAG, MPI_COMM_WORLD);
		
		//receive the ghost from rank 1
		MPI_Recv(&temp, 1026, cellDatatype, 1, TAG, MPI_COMM_WORLD, &status);
		//put the received ghost cell into row 1
		for(int i =0; i < 1026; i++)
		{
			if(temp[i].type != 0) //by doing this, some object will disappear 
			{
				mesh[i][1].type = temp[i].type;
				mesh[i][1].gender = temp[i].gender;
				mesh[i][1].age = temp[i].age;
			}
		}
		
		//clear out ghost cell
		for(int i = 0;i<1026;i++)
			mesh[i][0].type = 0;
	}
	if(rank == 1)
	{
		//receive the ghost cell pass from rank 0
		MPI_Recv(&temp, 1026, cellDatatype, 0, TAG, MPI_COMM_WORLD, &status);
		//put the received ghost cell into row SIZEJ 
		for(int i =0; i < 1026; i++)
		{
			if(temp[i].type != 0) //by doing this, some object will disappear 
			{
				mesh[i][SIZEJ].type = temp[i].type;
				mesh[i][SIZEJ].gender = temp[i].gender;
				mesh[i][SIZEJ].age = temp[i].age;
			}
		}
		
		//get the ghost cell
		for(int i =0; i<1026; i++)
		{
			temp[i].type = mesh[i][SIZEJ+1].type;
			temp[i].gender = mesh[i][SIZEJ+1].gender;
			temp[i].age = mesh[i][SIZEJ+1].age;
		}
		MPI_Send(&temp, 1026, cellDatatype, 0, TAG, MPI_COMM_WORLD);
			
		//clear out ghost cell
		for(int i = 0;i<1026;i++)
			mesh[i][SIZEJ+1].type = 0;
	}
	
	
}

void clearOutGhostcell(int rank,object (*mesh)[SIZEJ+2],MPI_Datatype cellDatatype)
{
	if(rank == 0)
		for(int i = 0;i<1026;i++)
			mesh[i][0].type = 0;
	if(rank == 1)
		for(int i = 0;i<1026;i++)
			mesh[i][SIZEJ+1].type = 0;
}