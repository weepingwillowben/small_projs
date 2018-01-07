#define MAT_SIZE 1024
#define VEC_SIZE 4
#define UNROLL_SIZE 4
__kernel void zero_buffer(__global float * buf){
	buf[get_global_id(0)] = 0;
}

__kernel void mat_mult ( __global float * buf1,__global float * buf2,__global float * buf3) {
	uint x = get_global_id(0);
	uint y = get_global_id(1)*VEC_SIZE;
	
	float res[VEC_SIZE] = {0,0,0,0};
	for(int ist = 0; ist <= MAT_SIZE - UNROLL_SIZE; ist += UNROLL_SIZE){
		float arr[UNROLL_SIZE];
		for(int i = 0; i < UNROLL_SIZE; i++){
			arr[i] = buf2[(ist+i)*MAT_SIZE+x];
		}
		for(int j = 0; j < VEC_SIZE; j++){	
			for(int k = 0; k < UNROLL_SIZE; k++){
				uint i = ist+k;
				res[j] += buf1[(j+y)*MAT_SIZE+i] * arr[k];
			}
		}
	}
	for(int j = 0; j < VEC_SIZE; j++){
		buf3[(j+y)*MAT_SIZE+x] = res[j];
	}
}

