#!/bin/bash
gcc tester.c rwlock.c -o rwlock-reader-pref -pthread
gcc tester.c rwlock.c -o rwlock-writer-pref -pthread

echo "Generating output in outputs-reader-pref"
mkdir outputs-reader-pref
./rwlock-reader-pref 5 0 > outputs-reader-pref/output_5_0
./rwlock-reader-pref 5 1 > outputs-reader-pref/output_5_1
./rwlock-reader-pref 5 3 > outputs-reader-pref/output_5_3
./rwlock-reader-pref 0 4 > outputs-reader-pref/output_0_4

echo "Grading output in outputs-reader-pref"
python autograder-reader-pref.py 

echo "Generating output in outputs-writer-pref"
mkdir outputs-writer-pref
./rwlock-writer-pref 5 0 > outputs-writer-pref/output_5_0
./rwlock-writer-pref 5 1 > outputs-writer-pref/output_5_1
./rwlock-writer-pref 5 3 > outputs-writer-pref/output_5_3
./rwlock-writer-pref 0 4 > outputs-writer-pref/output_0_4

echo "Grading output in outputs-writer-pref"
python autograder-writer-pref.py 
