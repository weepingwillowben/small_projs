extern "C"{
#include <immintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
}
float sum_mul(float *__restrict__  first,float *__restrict__  second,int size){
    float this_val = 0;
    int loc = 0;
    if(size > vec_size){
		/*asm(
		"vxorps %%ymm0,%%ymm0,%%ymm0\n\
		vxorps %%ymm1,%%ymm0,%%ymm0\n\
		vxorps %%ymm2,%%ymm0,%%ymm0\n\
		vxorps %%ymm3,%%ymm0,%%ymm0\n\
		movq 
		"
		);*/
        __m256 val1 = _mm256_setzero_ps();
        __m256 val2 = _mm256_setzero_ps();
        __m256 val3 = _mm256_setzero_ps();
        __m256 val4 = _mm256_setzero_ps();
        for(int j = 0; j < size-vec_size; j += vec_size){
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

            temp1 = _mm256_add_ps(val1,temp1);
            temp2 = _mm256_add_ps(val2,temp2);
            temp3 = _mm256_add_ps(val3,temp3);
            temp4 = _mm256_add_ps(val4,temp4);

            loc = j+vec_size;
        }
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
