all: project2

project2: project2.o student2_common.o student2A.o student2B.o
	gcc project2.o student2_common.o student2A.o student2B.o -o project2

project2.o: project2.c project2.h
	gcc -c project2.c -w

student2_common.o: student2_common.c project2.h
	gcc -c student2_common.c

student2A.o: student2A.c project2.h
	gcc -c student2A.c

student2B.o: student2B.c project2.h
	gcc -c student2B.c

clean:
	rm *.o project2