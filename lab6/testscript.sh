#!/bin/bash
# Lab 6: Dijkstra's Algorithm
# David Lynge
# dlynge
# ECE 2230, Fall 2018

rm -f testoutput.txt

echo "Starting test script ..." >> testoutput.txt

echo "TESTING EACH GRAPH TYPE/OPERATION" >> testoutput.txt

./lab6 -g 1 -h 1 -s 1 -d 5 >> testoutput.txt
./lab6 -g 2 -h 1 -s 0 -d 50 -n 100 >> testoutput.txt
./lab6 -g 3 -h 1 -s 0 -d 50 -n 100 -a 15 >> testoutput.txt

./lab6 -g 1 -h 2 >> testoutput.txt
./lab6 -g 2 -h 2 -n 100 >> testoutput.txt
./lab6 -g 3 -h 2 -n 100 -a 15 >> testoutput.txt

./lab6 -g 1 -h 3 -s 1 -d 5 >> testoutput.txt
./lab6 -g 2 -h 3 -s 0 -d 50 -n 100 >> testoutput.txt
./lab6 -g 3 -h 3 -s 0 -d 50 -n 100 -a 15 >> testoutput.txt

echo >> testoutput.txt
echo "EXTREME CASES" >> testoutput.txt

./lab6 -g 1 -h 1 -s -11 -d 5 >> testoutput.txt
./lab6 -g 3 -h 2 -n 10 -a 15 >> testoutput.txt
./lab6 -g 3 -h 3 -s 0 -d 150 -n 100 -a 15 >> testoutput.txt

echo >> testoutput.txt
echo "PERFORMANCE EVALUATION" >> testoutput.txt

echo "(1.a) N vs N/2 time complexity w/ -g 2" >> testoutput.txt
./lab6 -g 2 -h 1 -n 20000 -s 0 -d 19999 >> testoutput.txt
./lab6 -g 2 -h 1 -n 10000 -s 0 -d 9999 >> testoutput.txt

echo "(1.b) N vs N/2 time complexity w/ -g 3" >> testoutput.txt
./lab6 -g 3 -h 1 -n 15000 -s 0 -d 14999 -a 20 >> testoutput.txt
./lab6 -g 3 -h 1 -n 7500 -s 0 -d 7499 -a 20 >> testoutput.txt

echo "(2) Network diameter complexity" >> testoutput.txt
./lab6 -g 3 -h 2 -n 300 -a 20 >> testoutput.txt
./lab6 -g 3 -h 2 -n 600 -a 20 >> testoutput.txt

echo "(3) Node density for random graphs" >> testoutput.txt
SEED=0
while [ $SEED -lt 10 ]; do
    ./lab6 -g 3 -h 2 -n 50 -r $SEED -a 7 >> testoutput.txt
    let SEED=SEED+1
done
while [ $SEED -lt 20 ]; do
    ./lab6 -g 3 -h 2 -n 50 -r $SEED -a 20 >> testoutput.txt
    let SEED=SEED+1
done

echo "(4.a) Multiple link-disjoint paths w/ -g 2" >> testoutput.txt
./lab6 -g 2 -h 3 -n 15 -s 14 -d 0 >> testoutput.txt

echo "(4.b) Multiple link-disjoint paths w/ -g 3" >> testoutput.txt
./lab6 -g 3 -h 3 -n 1000 -s 0 -d 999 -a 10 >> testoutput.txt
./lab6 -g 3 -h 3 -n 1000 -s 0 -d 999 -a 20 >> testoutput.txt
./lab6 -g 3 -h 3 -n 1000 -s 0 -d 999 -a 50 >> testoutput.txt
./lab6 -g 3 -h 3 -n 1000 -s 0 -d 999 -a 100 >> testoutput.txt

cat testoutput.txt
