#/bin/bash

cd fann/; 
make clean;  rm -r CMakeFiles; rm CMakeCache.txt
cmake  .; make
ar -rcs src/libfann.a src/CMakeFiles/floatfann.dir/floatfann.c.o
