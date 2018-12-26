#include "globals.h"
#ifdef avx
extern "C"{
#include <immintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
}
float sum_mul(float *__restrict__  first,float *__restrict__  second,int size){
    float this_val = 0;
    int loc = 0;
    if(size > vec_size){

        /*float * end_unrolled = size-vec_size + first;
        int VecSize = vec_size * sizeof(float);
        asm(
        "vxorps %%ymm0,%%ymm0,%%ymm0\n\
        vxorps %%ymm1,%%ymm0,%%ymm0\n\
        vxorps %%ymm2,%%ymm0,%%ymm0\n\
        vxorps %%ymm3,%%ymm0,%%ymm0\n\
        movq %[first],%%rdi
        movq %[second],%%rsi
        vector_loop:
        addq %[VecSize],%%rdi
        addq %[VecSize],%%rsi
        cmpq %%rdi,%[end_unrolled]
        jle end_vector_loop
        vmovups %%ymm4,(%%rdi)
        vmovups %%ymm5,(%%rsi)
        vmovups %%ymm6,32(%%rdi)
        vmovups %%ymm7,32(%%rsi)
        vmovups %%ymm8,64(%%rdi)
        vmovups %%ymm9,64(%%rsi)
        vmovups %%ymm10,96(%%rdi)
        vmovups %%ymm11,96(%%rsi)


        vmulps  %%ymm12,%%ymm4,%%ymm5
        vmulps  %%ymm13,%%ymm6,%%ymm7
        vmulps  %%ymm14,%%ymm8,%%ymm9
        vmulps  %%ymm15,%%ymm10,%%ymm11

        vaddps  %%ymm12,%%ymm4,%%ymm5
        vaddps  %%ymm13,%%ymm6,%%ymm7
        vaddps  %%ymm14,%%ymm8,%%ymm9
        vaddps  %%ymm15,%%ymm10,%%ymm11

        end_vector_loop:
        subq %[first],%%rdi
        subq %[VecSize],%%rdi
        movq %%rdi,%[loc]
        "
        );*/

        __m256 val1 = _mm256_setzero_ps();
        __m256 val2 = _mm256_setzero_ps();
        __m256 val3 = _mm256_setzero_ps();
        __m256 val4 = _mm256_setzero_ps();
        int j = 0;
        for(; j < size-vec_size; j += vec_size){
            __m256 fst1 = _mm256_loadu_ps(first + j);
            __m256 sec1 = _mm256_loadu_ps(second + j);
            __m256 fst2 = _mm256_loadu_ps(first + j + 8);
            __m256 sec2 = _mm256_loadu_ps(second + j + 8);
            __m256 fst3 = _mm256_loadu_ps(first + j + 16);
            __m256 sec3 = _mm256_loadu_ps(second + j + 16);
            __m256 fst4 = _mm256_loadu_ps(first + j + 24);
            __m256 sec4 = _mm256_loadu_ps(second + j + 24);

            __m256 temp1 = _mm256_mul_ps(fst1,sec1);
            __m256 temp2 = _mm256_mul_ps(fst2,sec2);
            __m256 temp3 = _mm256_mul_ps(fst3,sec3);
            __m256 temp4 = _mm256_mul_ps(fst4,sec4);

            val1 = _mm256_add_ps(val1,temp1);
            val2 = _mm256_add_ps(val2,temp2);
            val3 = _mm256_add_ps(val3,temp3);
            val4 = _mm256_add_ps(val4,temp4);
        }
        loc = j-vec_size;

        val1 = _mm256_add_ps(val1,val2);
        val3 = _mm256_add_ps(val3,val4);
        val1 = _mm256_add_ps(val1,val3);
        __m128 top = _mm256_extractf128_ps(val1,1);
        __m128 bottom = _mm256_extractf128_ps(val1,0);

        top = _mm_add_ps(top,bottom);

        float val_vec[4];
        _mm_storeu_ps(val_vec,top);

        val_vec[0] += val_vec[2];
        val_vec[1] += val_vec[3];

        this_val = val_vec[0] + val_vec[1];
    }
    for(int j = loc; j < size; j++){
        this_val += first[j] * second[j];
    }
    return this_val;
}
void add_with_mul(float *__restrict__  out,float *__restrict__  in,float mul_val,int size){
    __m256 mul_val_vec = _mm256_set1_ps(mul_val);
    int loc = 0;
    if(size > vec_size){
        int j = 0;
        for(; j < size-vec_size; j += vec_size){
            __m256 in1 = _mm256_loadu_ps(in + j);
            __m256 out1 = _mm256_loadu_ps(out + j);
            __m256 in2 = _mm256_loadu_ps(in + j + 8);
            __m256 out2 = _mm256_loadu_ps(out + j + 8);
            __m256 in3 = _mm256_loadu_ps(in + j + 16);
            __m256 out3 = _mm256_loadu_ps(out + j + 16);
            __m256 in4 = _mm256_loadu_ps(in + j + 24);
            __m256 out4 = _mm256_loadu_ps(out + j + 24);

            __m256 temp1 = _mm256_mul_ps(in1,mul_val_vec);
            __m256 temp2 = _mm256_mul_ps(in2,mul_val_vec);
            __m256 temp3 = _mm256_mul_ps(in3,mul_val_vec);
            __m256 temp4 = _mm256_mul_ps(in4,mul_val_vec);

            out1 = _mm256_add_ps(out1,temp1);
            out2 = _mm256_add_ps(out2,temp2);
            out3 = _mm256_add_ps(out3,temp3);
            out4 = _mm256_add_ps(out4,temp4);

            _mm256_storeu_ps(out + j,out1);
            _mm256_storeu_ps(out + j + 8,out2);
            _mm256_storeu_ps(out + j + 16,out3);
            _mm256_storeu_ps(out + j + 24,out4);
        }
        loc = j - vec_size;
    }
    for(int j = loc; j < size; j++){
        out[j] += in[j] * mul_val;
    }
}
#else
#ifdef optimized
float sum_mul(float *__restrict__  first,float *__restrict__  second,int size){
    float this_val = 0;
    int loc = 0;
    if(size > vec_size){
        float val_vec[vec_size] = {0,0,0,0,0,0,0,0,
                                   0,0,0,0,0,0,0,0};

        for(int j = 0; j < size-vec_size; j += vec_size){
            val_vec[0] += first[j] * second[j];
            val_vec[1] += first[j+1] * second[j+1];
            val_vec[2] += first[j+2] * second[j+2];
            val_vec[3] += first[j+3] * second[j+3];
            val_vec[4] += first[j+4] * second[j+4];
            val_vec[5] += first[j+5] * second[j+5];
            val_vec[6] += first[j+6] * second[j+6];
            val_vec[7] += first[j+7] * second[j+7];

            val_vec[8] += first[j+8] * second[j+8];
            val_vec[9] += first[j+9] * second[j+9];
            val_vec[10] += first[j+10] * second[j+10];
            val_vec[11] += first[j+11] * second[j+11];
            val_vec[12] += first[j+12] * second[j+12];
            val_vec[13] += first[j+13] * second[j+13];
            val_vec[14] += first[j+14] * second[j+14];
            val_vec[15] += first[j+15] * second[j+15];
            loc = j+vec_size;
        }
        val_vec[0] += val_vec[8];
        val_vec[1] += val_vec[9];
        val_vec[2] += val_vec[10];
        val_vec[3] += val_vec[11];
        val_vec[4] += val_vec[12];
        val_vec[5] += val_vec[13];
        val_vec[6] += val_vec[14];
        val_vec[7] += val_vec[15];

        val_vec[0] += val_vec[4];
        val_vec[1] += val_vec[5];
        val_vec[2] += val_vec[6];
        val_vec[3] += val_vec[7];

        val_vec[0] += val_vec[2];
        val_vec[1] += val_vec[3];

        this_val += val_vec[0] + val_vec[1];
    }
    for(int j = loc; j < size; j++){
        this_val += first[j] * second[j];
    }
    return this_val;
}
void add_with_mul(float *__restrict__  out,float *__restrict__  in,float mul_val,int size){
    float val_vec[vec_size] = {mul_val,mul_val,mul_val,mul_val,mul_val,mul_val,mul_val,mul_val,
                               mul_val,mul_val,mul_val,mul_val,mul_val,mul_val,mul_val,mul_val};
    int loc = 0;
    if(size > vec_size){
        for(int j = 0; j < size-vec_size; j += vec_size){
            out[j] += val_vec[0] * in[j];
            out[j+1] += val_vec[1] * in[j+1];
            out[j+2] += val_vec[2] * in[j+2];
            out[j+3] += val_vec[3] * in[j+3];
            out[j+4] += val_vec[4] * in[j+4];
            out[j+5] += val_vec[5] * in[j+5];
            out[j+6] += val_vec[6] * in[j+6];
            out[j+7] += val_vec[7] * in[j+7];

            out[j+8] += val_vec[8] * in[j+8];
            out[j+9] += val_vec[9] * in[j+9];
            out[j+10] += val_vec[10] * in[j+10];
            out[j+11] += val_vec[11] * in[j+11];
            out[j+12] += val_vec[12] * in[j+12];
            out[j+13] += val_vec[13] * in[j+13];
            out[j+14] += val_vec[14] * in[j+14];
            out[j+15] += val_vec[15] * in[j+15];
            loc = j + vec_size;
        }
    }
    for(int j = loc; j < size; j++){
        out[j] += in[j] * mul_val;
    }
}
#else
float sum_mul(float *__restrict__  first,float *__restrict__  second,int size){
    float this_val = 0;
    for(int j = 0; j < size; j++){
        this_val += first[j] * second[j];
    }
    return this_val;
}
void add_with_mul(float *__restrict__  out,float *__restrict__  in,float mul_val,int size){
    for(int j = 0; j < size; j++){
        out[j] += mul_val * in[j];
    }
}

#endif
#endif
