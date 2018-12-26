//#include "C:/Users/Benjamin/Documents/PythonProjects/neural_nets/net_prac/net_algorithms.hpp"
/*__kernel void test(__global float *x) {
  __local xcopy[GROUP_SIZE];

  int globalid = get_global_id(0);
  int localid = get_local_id(0);
  event_t e = async_work_group_copy(xcopy, x+globalid-localid, GROUP_SIZE, 0);
  wait_group_events(1, &e);
}*/
kernel void gradient_descent_weights(global float * weights,global float * prev_zs,global float * prev_activs,global float * error,global float * prev_error,float lambda,float regularzing_weight_const,int size,int prev_size){
	uint k = get_global_id(0);
	float p_err = 0;
	float p_activs = prev_activs[k];
	for(int j = 0; j < size; j++){
		float this_err = error[j];
		float adj_error = this_err * lambda;

		weights[k+j * prev_size] *= regularzing_weight_const;
		weights[k+j * prev_size] += -adj_error * p_activs;

		//backpropogates error
		p_err += this_err * weights[k+j * prev_size];
	}
    prev_error[k] = p_err * activ_fn_deriv(prev_zs[k]);
}
kernel void activate(global float * prev_activs,global float * weights,global float * biases,global float * zs,global float * activs,int size){
	uint k = get_global_id(0);
	global float * this_weights = weights + k * size;

	float this_val = biases[k];
	for(int j = 0; j < size; j++){
		this_val += prev_activs[j] * this_weights[j];
	}
	zs[k] = this_val;
	activs[k] = activ_fn(this_val);
}
kernel void set_output_error(global float * error,global float * activs,global float * expected_activs){
	uint j = get_global_id(0);
	error[j] = activs[j]-expected_activs[j];
}
kernel void add_bias_error(global float * biases,global float * error,float lambda){
	uint j = get_global_id(0);
	biases[j] += -error[j]*lambda;
}





