# The makefile for MP4.
# David Lynge
# Type:
#   make         -- to build program lab4
#   make clean   -- to delete object files, executable, and core
#   make list.o  -- to compile only list.o (or: use lab4.o)

lab4 : list.o lab4.o mem.o
	gcc -Wall -g list.o lab4.o mem.o -o lab4

list.o : list.c datatypes.h list.h
	gcc -Wall -g -c list.c

mem.o : mem.c datatypes.h mem.h
	gcc -Wall -g -c mem.c

lab4.o : lab4.c datatypes.h list.h
	gcc -Wall -g -c lab4.c

clean :
	rm -f *.o lab4 a.out

