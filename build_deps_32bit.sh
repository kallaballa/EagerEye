#/bin/bash

cd fann/; make clean;  rm -r CMakeFiles; rm CMakeCache.txt
cd fann; cmake  -DCMAKE_C_FLAGS="-m32" .; make
