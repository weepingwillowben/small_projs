
class layer{
public:
	layer()=default;
	layer(int in_size,int in_prev_size,float in_lambda,float in_reg_param):
			lambda(in_lambda),
			reg_param(in_reg_param),
			size(in_size),
			biases(size),
			weights(size * in_prev_size),
			activs(size),
			zs(size),
			error(size)
	{
		for(float & f : biases)
			f = rand_bias();
		for(float & w : weights)
			w = rand_weight(in_prev_size);
	}

	 ~layer() = default;
	float lambda;
	float reg_param;
	int size;
	vector<float> biases;
	vector<float> weights;
	vector<float> activs;
	vector<float> zs;
	vector<float> error;
	void activate(layer * prev){
		for(int k = 0; k < size; k++){
			float this_val = biases[k];
			for (int j = 0; j < prev->size; j++)
				this_val += prev->activs[j] * weights[j+k * prev->size];

			zs[k] = this_val;
			activs[k] = activ_fn(this_val);
		}
	}
	void gradient_descent(layer * prev){
		const float adj_lambda = lambda / size;
		const float regularzing_weight_const = 1 - adj_lambda * reg_param;
		for(int k = 0; k < prev->size; k++){
			float p_error = 0;
			float p_activs = prev->activs[k];
			for(int j = 0; j < size; j++){
				float adj_error = error[j] * adj_lambda;

				float & this_weight = weights[k+j * prev->size];
				this_weight *= regularzing_weight_const;
				this_weight += -adj_error * p_activs;

				//backpropogates error
				p_error += error[j] * this_weight;
			}
			prev->error[k] = p_error * activ_fn_deriv(prev->zs[k]);
		}
		for(int k = 0;k < size; k++)
			biases[k] += -error[k] * adj_lambda;
	}
};

class output_layer:
	public layer
{
public:
	output_layer(int size,int prev_size,float in_lambda,float in_reg_param):
		layer(size,prev_size,in_lambda,in_reg_param)
	{}
	output_layer()=default;

	test_info get_correct(vector<float> & expected_activs){
		float max_val = -1000000.0;
		int max_num = -1;
		int expected_num = -1;
		float sum_diffs_sqrd = 0;
		for(int j = 0; j < size;j++){
			float this_val = activs[j];
			float exp_val = expected_activs[j];
			sum_diffs_sqrd += sqare(exp_val - this_val);
			if(this_val > max_val){
				max_val = this_val;
				max_num = j;
			}
			if (exp_val == float(1))
				expected_num = j;
		}
		if(max_num == -1 || expected_num == -1)
			cout << "error!\n";

		float tot_cost = sqrt(sum_diffs_sqrd);
		bool is_accurate = expected_num == max_num;

		return test_info{tot_cost,is_accurate};
	}

	void back_prop_output(vector<float> & expected_activs){
		for(int j = 0; j < size; j++){
			//float is_correct = expected[j];
			//float act_deriv = activ_fn_deriv(zs[j]);
			//float cost_d = cost_deriv(activations[j],is_correct);
			float err = activs[j]-expected_activs[j];//cost_d * act_deriv;
			if (err != err)
				cout <<"arg!";
			error[j] = err;
		}
	}
};
