#!/bin/bash
# Lab 7: Hash Tables
# David Lynge
# dlynge
# ECE 2230, Fall 2018

echo "Testing rehash driver"
./lab7 -b -h quad
./lab7 -b -h linear
./lab7 -b -h double

echo "Testing unit drivers"
./lab7 -u 1
./lab7 -u 2
./lab7 -u 3

echo "Testing receive driver"
./lab7 -r -a 0.85 -h quad -m 65536 -i rand
./lab7 -r -a 0.85 -h quad -m 65536 -i seq
./lab7 -r -a 0.85 -h quad -m 65536 -i fold
./lab7 -r -a 0.85 -h quad -m 65536 -i worst
./lab7 -r -a 0.85 -h double -m 65537 -i rand
./lab7 -r -a 0.85 -h double -m 65537 -i seq
./lab7 -r -a 0.85 -h double -m 65537 -i fold
./lab7 -r -a 0.85 -h double -m 65537 -i worst
./lab7 -r -a 0.85 -h linear -m 65537 -i rand
./lab7 -r -a 0.85 -h linear -m 65537 -i seq
./lab7 -r -a 0.85 -h linear -m 65537 -i fold
./lab7 -r -a 0.85 -h linear -m 65537 -i worst

echo "Testing equilibrium driver"
./lab7 -e -a 0.85 -h linear -m 65537 -t 50000
./lab7 -e -a 0.85 -h linear -m 65537 -t 100000


