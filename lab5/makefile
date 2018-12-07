# The makefile for MP5.
# David Lynge
# dlynge
# Type:
#   make         -- to build program lab5
#   make clean   -- to delete object files, executable, and core
#   make bst.o  -- to compile only list.o (or: use lab5.o)

lab5 : bst.o lab5.o
	gcc -Wall -g bst.o lab5.o -o lab5 -lm

bst.o : bst.c bst.h
	gcc -Wall -g -c bst.c

lab5.o : lab5.c bst.h
	gcc -Wall -g -c lab5.c

clean :
	rm -f *.o lab5 a.out

