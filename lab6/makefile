# Makefile for MP6
# David Lynge
# dlynge
# ECE 2230, Fall 2018

lab6 : graph.o lab6.o
	gcc -Wall -g graph.o lab6.o -o lab6 -lm

graph.o : graph.c graph.h
	gcc -Wall -g -c graph.c

lab6.o : lab6.c
	gcc -Wall -g -c lab6.c

clean :
	rm -f *.o a.out lab6

