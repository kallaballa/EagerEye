#/bin/bash

cd fann/; make clean;  rm -r CMakeFiles; rm CMakeCache.txt
cd fann; cmake  .; make
ar -rcs fann/src/libfann.a fann/src/CMakeFiles/floatfann.dir/floatfann.c.o
