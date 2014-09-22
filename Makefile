HDR = util.h datatype.h mesh.h

OBJ = zim.o util.o datatype.o mesh.o

CC  = gcc -Wall -fopenmp -g -std=c99 -lm

zim: $(OBJ)
	$(CC) -o zim $(OBJ)
	
clean:
	/bin/rm $(OBJ)

$(OBJ):	$(HDR)