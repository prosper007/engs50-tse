#!/bin/bash

# A simple Shell Script that applies Valgrind to all test programs and
# ensurses that there are no memory leaks or errors

make
for f in test_*; do
		valgrind --leak-check=full ${f} > tmp.txt 2>&1
    LEAK=$(grep 'no leaks are possible' tmp.txt)
    ERR=$(grep '0 errors' tmp.txt)
		cat tmp.txt
    if [ -z "$LEAK" ] || [ -z "$ERR" ]; then
				echo "memory leak or error present in file ${f}"
    else
				echo "no leaks or errors present in file ${f}"
    fi
    rm tmp.txt
		echo
		echo "================"
		echo
done
make clean

