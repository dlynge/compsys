#!/bin/sh
# lab5.c
# Lab5: Binary Search Trees
# ECE 2230, Fall 2018
# David Lynge
# dlynge
# This file contains the test script for lab5


./lab5 -u 0			#// example test to remove leaves, 12 and 20, then internal nodes
./lab5 -u 1			#// (48) is missing its right-left child and (16) is missing its left
./lab5 -u 2			#// example tests: (16) is missing its right-left child and (48) is missing its left
./lab5 -u 3			#// example deletion with many children
./lab5 -u 4			#// check replace for duplicate key
./lab5 -u 5			#// example from homework set
./lab5 -u 6			#// example with poor shape that improves wtih balancing 
./lab5 -u 7			#// example removing root and removing the new root
./lab5 -u 8			#// example replacing root and then removing it
./lab5 -u 9			#// example replacing a leaf and removing it
./lab5 -o -w 5 -t 0 -v			#// tests inserts only and prints tree
./lab5 -r -w 5 -t 0 -v -s 1			#// same with random tree
./lab5 -p -w 5 -t 0 -v -s 1			#// same with poor tree
./lab5 -o -w 16 -t 50000			#// tests inserts and accesses
./lab5 -r -w 16 -t 50000			#// same with random tree
./lab5 -p -w 16 -t 50000			#// same with poor tree
./lab5 -e -w 20 -t 100000			#// exercise the tree

