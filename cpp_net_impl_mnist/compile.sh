g++  main.cpp -std=c++11 -I ~/"Visual Studio 2013"/ -I "C:\Users\Benjamin\Downloads\boost_1_56_0\boost_1_56_0" -O3 -march=native -mtune=native -ffast-math -mavx -S -o gcc_code.s
#clang++  main.cpp -std=c++11 -I ~/"Visual Studio 2013"/ -O3 -march=native -mtune=native -ffast-math -mavx -S -o clang_code.s
#compile exp_intrin.c
#gcc exp_intrin.c -c -o exp_intrin_cyg.o -O3 -mavx -march=native -mtune=native
#g++  main.cpp -l exp_intrin_cyg.o -std=c++11 -I ~/"Visual Studio 2013"/ -I "C:\Users\Benjamin\Downloads\boost_1_56_0\boost_1_56_0" -O3 -march=native -mtune=native -ffast-math -mavx -fopenmp -o gcc_code.exe
