#include <thread>
#include <iostream>
#include "immintrin.h"

#include "mat_mult.h"

using namespace std;

constexpr size_t num_vecs = 8;
constexpr size_t vec_size = 8;
constexpr size_t lin_size = num_vecs * vec_size;
constexpr size_t matsize = 1024;


void cpu_mat_mult(vector<float> & mat1,vector<float> & mat2,vector<float> & matres){
    auto yfn = [&](size_t ystart,size_t yend){
        for(size_t y = ystart; y < yend; y++){
            size_t x = 0;
            for(; x <= matsize-lin_size; x += lin_size){
                __m256 res[num_vecs];
                for(size_t i = 0; i < num_vecs; i++){
                    res[i] = _mm256_setzero_ps();
                }
                for(size_t j = 0; j < matsize; j++){
                    float * mat2sec = &mat2[j*matsize+x];
                    __m256 v1 = _mm256_set1_ps(mat1[y*matsize+j]);
                    for(size_t i = 0; i < num_vecs; i++){
                        res[i] = _mm256_add_ps(_mm256_mul_ps(_mm256_loadu_ps(mat2sec + i*vec_size),v1),res[i]);
                    }
                }
                float * res_sec = &matres[y*matsize+x];
                for(size_t i = 0; i < num_vecs; i++){
                    _mm256_storeu_ps(res_sec+i*vec_size,res[i]);
                }
            }
            for(; x < matsize; x++){
                float res = 0;
                for(size_t i = 0; i < matsize; i++){
                    res += mat1[y*matsize+i] * mat2[i*matsize+x];
                }
                matres[y*matsize+x] = res;
            }
        }
    };
    
    int64_t num_threads = thread::hardware_concurrency();
    vector<thread> ts;
    ts.reserve(num_threads);
    
    size_t max_block_size = (matsize / num_threads);
    size_t xstart = 0;
    for(int64_t i = 0; i < num_threads; i++){
        size_t xend = min(matsize,xstart + max_block_size);
        ts.emplace_back(yfn,xstart,xend);
        xstart = xend;
    }
    for(int64_t i = 0; i < num_threads; i++){
        ts[i].join();
    }
}
