#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <ctime>
#include <fstream>
#include <cstdlib>
#include <random>
#include <inttypes.h>
#include <functional>
#include <string>
#include <deque>
#include <memory>
#include <windows.h>
#include "RangeIterator.h"
#include "backwards_iter.h"
#include "intrinsic_help.h"
typedef unsigned char BYTE;
#define not_optimized
#define avx
#define OPENCL_n

#include <boost/align/aligned_allocator.hpp>
extern "C"{
//#include <immintrin.h>
//#include <emmintrin.h>
void exp256_ps(float * val256);
}
//increase this to 40 to get a little speed boost
#define BATCH_SIZE 32
using namespace std;
extern std::mt19937_64 generator;

template<typename numty>
inline numty sqare(numty val){
    return val * val;
}
inline float extremity_of(vector<float> & vec){
    float sum = 0;
    for(float & d : vec)
        sum += d;
    return sum / float(vec.size());
}
inline float GetCounter(){
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    LARGE_INTEGER ticks;
    QueryPerformanceCounter(&ticks);
    return float(ticks.QuadPart)/freq.QuadPart;
}
struct test_info{
    float tot_cost;
    int tot_terms;
    void operator += (test_info other){
        tot_cost += other.tot_cost;
        tot_terms += other.tot_terms;
    }
};
inline float rand_weight(int size){
    std::normal_distribution<float> distribution(0.0,1.0/sqrt(float(size)));
    return distribution(generator);
}
inline float rand_bias(){
    return 0;
}
