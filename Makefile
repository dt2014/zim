HDR = util.h datatype.h mesh.h MPI_Activity.h

OBJ = zim.o util.o datatype.o mesh.o  MPI_Activity.o

CC  = mpicc -Wall -fopenmp -g -std=c99 -lm

zim: $(OBJ)
	$(CC) -o zim $(OBJ)
	
clean:
	/bin/rm $(OBJ)

$(OBJ):	$(HDR)