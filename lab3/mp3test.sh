#!/bin/sh
# mp3test.sh
# David Lynge
# ECE 2230, Fall 2018
#
# A simple shell script for MP3 to generate data for performance analysis
#
# This script should run in the range from about 12 to 30 seconds.  If the
#    run time is much longer then there is a bug in your lab3 sorts.  If
#    the run time is much shorter, then you need to increase the size
#    of the lists (see "sizes" below).
#
# There are two options to execute this file: either make the file executable
#    or run the file in a new shell.
#
# Option 1:  In a terminal type:
#      chmod +x mp3test.sh
#      ./mp3test.sh
#
# Option 2: run the program sh (this is a command shell) and tell sh to read
#    the commands from the file mp3test.sh
#     sh mp3test.sh
#
# There are four tests for:
#    Insertion Sort, Recursive Selection Sort, Iterative Selection Sort,
#    and Merge Sort. 
#
# Each sort is run with random, ascending, and decending lists.
# 
# "sizes" is used to hold the size for each trial
#
# TEST 1 Insertion sort
echo "The date today is `date`"
echo "insertion sort with random list"
sizes="2000 8000 16000 24000 32000"
for listsize in $sizes ; do
   ./geninput $listsize 1 1 | ./lab3
done
echo "insertion sort with ascending list"
for listsize in $sizes ; do
   ./geninput $listsize 2 1 | ./lab3
done
echo "insertion sort with descending list"
for listsize in $sizes ; do
   ./geninput $listsize 3 1 | ./lab3
done
#
# TEST 2 Recursive selection sort
echo "recursive selection sort with random list"
for listsize in $sizes ; do
   ./geninput $listsize 1 2 | ./lab3
done
echo "recursive selection sort with ascending list"
for listsize in $sizes ; do
   ./geninput $listsize 2 2 | ./lab3
done
echo "recursive selection sort with descending list"
for listsize in $sizes ; do
   ./geninput $listsize 3 2 | ./lab3
done
#
# TEST 3 Iterative selection sort
echo "iterative selection sort with random list"
for listsize in $sizes ; do
   ./geninput $listsize 1 3 | ./lab3
done
echo "iterative selection sort with ascending list"
for listsize in $sizes ; do
   ./geninput $listsize 2 3 | ./lab3
done
echo "iterative selection sort with descending list"
for listsize in $sizes ; do
   ./geninput $listsize 3 3 | ./lab3
done
#
# TEST 4 Merge sort
echo "merge sort with random list"
sizes="32000 200000 400000 600000 800000"
for listsize in $sizes ; do
   ./geninput $listsize 1 4 | ./lab3
done
echo "merge sort with ascending list"
for listsize in $sizes ; do
   ./geninput $listsize 2 4 | ./lab3
done
echo "merge sort with descending list"
for listsize in $sizes ; do
   ./geninput $listsize 3 4 | ./lab3
done
#
echo "end"

