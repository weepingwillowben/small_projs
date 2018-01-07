g++ -std=c++11 -O3 -march=haswell -mtune=haswell -c -o cpp.o mat_mult.cpp
#g++ -std=c++11 -O3 -march=haswell -mtune=haswell -S mat_mult.cpp
g++ -std=c++11 -O2 -I "." -c -o main.o main.cpp 
g++ -o test.exe main.o cpp.o C:/Windows/System32/OpenCL.dll