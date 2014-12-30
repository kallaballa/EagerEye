#/bin/bash

cd fann/; make clean;  rm -r CMakeFiles; rm CMakeCache.txt
cd fann; CC=emcc CXX=em++ cmake .; make

