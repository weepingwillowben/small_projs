#include "net_algorithms.hpp"
#include "globals.h"
using alloc_ty
#ifdef avx
 = boost::alignment::aligned_allocator<float,32>;
#else
 = allocator<float>;
#endif

class basic_layer{
public:
    basic_layer()=default;
    basic_layer(int in_size,int in_prev_size,float in_lambda,float in_reg_param):
        //todo: change this constructor so that it is more flexible, allowing for differnt sizes(batched and non-batched)
            size(in_size),
            biases(size),
            weights(size * in_prev_size),
            lambda(in_lambda),
            reg_param(in_reg_param)
    {
        for(float & f : biases)
            f = rand_bias();
        for(float & w : weights)
            w = rand_weight(in_prev_size);
    }

    virtual ~basic_layer() = default;

    float lambda;
    float reg_param;
    int size;
    vector<float,alloc_ty> biases;
    vector<float,alloc_ty> weights;
    vector<float,alloc_ty> activs;
    vector<float,alloc_ty> zs;
    vector<float,alloc_ty> error;

    virtual void gradient_descent_input(basic_layer * prev) = 0;
    virtual void gradient_descent(basic_layer * prev) = 0;
    virtual void activate(basic_layer * prev) = 0;
};
template<typename inherit_ty>
class output_layer_template:
    public inherit_ty
{
public:
    template<typename ...vartys>
    output_layer_template(vartys & ... args):
        inherit_ty(args...){}
    virtual ~output_layer_template()=default;

    virtual test_info get_correct(vector<float> & expected_activs) = 0;
    virtual void back_prop_output(vector<float> & expected_activs) = 0;
};

class normal_layer:
    public basic_layer
{
public:
    normal_layer()=default;
    virtual ~normal_layer()=default;
    normal_layer(int in_size,int in_prev_size,float in_lambda,float in_reg_param):
            basic_layer(in_size,in_prev_size,in_lambda,in_reg_param)
    {
        activs.resize(size);
        zs.resize(size);
        error.resize(size);
    }

    virtual void activate(basic_layer * prev){
        for(int k = 0; k < size; k++){
            float this_val = biases[k];
            for (int j = 0; j < prev->size; j++)
                this_val += prev->activs[j] * weights[j+k * prev->size];

            zs[k] = this_val;
            activs[k] = activ_fn(this_val);
        }
    }
    virtual void gradient_descent(basic_layer * prev){
        const float adj_lambda = lambda;// / size;
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
    virtual void gradient_descent_input(basic_layer * prev){
        const float adj_lambda = lambda;// / size;
        const float regularzing_weight_const = 1 - adj_lambda * reg_param;
        for(int k = 0; k < prev->size; k++){
            float p_activs = prev->activs[k];
            for(int j = 0; j < size; j++){
                float adj_error = error[j] * adj_lambda;

                float & this_weight = weights[k+j * prev->size];
                this_weight *= regularzing_weight_const;
                this_weight += -adj_error * p_activs;
            }
        }
        for(int k = 0;k < size; k++)
            biases[k] += -error[k] * adj_lambda;
    }
};
class normal_output_layer:
    public output_layer_template<normal_layer>
{
public:
    normal_output_layer(int size,int prev_size,float in_lambda,float in_reg_param):
        output_layer_template<normal_layer>(size,prev_size,in_lambda,in_reg_param)
    {}
    normal_output_layer()=default;
    virtual ~normal_output_layer()=default;

    virtual test_info get_correct(vector<float> & expected_activs){
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
    virtual void back_prop_output(vector<float> & expected_activs){
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

class batched_layer :
        public basic_layer{
public:
    batched_layer()=default;
    batched_layer(int in_size,int in_prev_size,float in_lambda,float in_reg_param){
        size = in_size;
        lambda = in_lambda;
        reg_param = in_reg_param;
        biases.resize(size);
        weights.resize(size * in_prev_size);
        activs.resize(size*BATCH_SIZE);
        zs.resize(size*BATCH_SIZE);
        error.resize(size*BATCH_SIZE);

        for(float & f : biases)
            f = rand_bias();
        for(float & w : weights)
            w = rand_weight(in_prev_size);
    }

    virtual ~batched_layer() = default;
    virtual void activate(basic_layer * prev){
        for(int k = 0; k < size; k++){
            block_vec actsum(biases[k]);
            for (int j = 0; j < prev->size; j++){
                float t_weight = weights[j+k * prev->size];
                actsum += block_vec(&prev->activs[j*BATCH_SIZE]) * fvec8(t_weight);
            }
            actsum.store(&zs[k*BATCH_SIZE]);
            block_vec res = activ_fn(actsum);
            res.store(&activs[k*BATCH_SIZE]);
        }
    }
    virtual void gradient_descent(basic_layer * prev){
        fvec8 adj_lam(-lambda);
        const float regularzing_weight_const = 1 - lambda * reg_param;
        for(int k = 0; k < prev->size; k++){
            block_vec perr;
            block_vec t_p_act(&prev->activs[k*BATCH_SIZE]);
            for(int j = 0; j < size; j++){
                float & this_weight = weights[k+j * prev->size];
                this_weight *= regularzing_weight_const;
                block_vec t_err(&error[j*BATCH_SIZE]);

                block_vec w_add = (t_err * adj_lam) * t_p_act;
                this_weight += w_add.sum();
                perr += t_err * fvec8(this_weight);
            }
            perr *= activ_fn_deriv(block_vec(&prev->zs[k*BATCH_SIZE]));
            perr.store(&prev->error[k*BATCH_SIZE]);
        }
        for(int k = 0;k < size; k++)
            biases[k] += (block_vec(&error[k*BATCH_SIZE]) * adj_lam).sum();
    }
    virtual void gradient_descent_input(basic_layer * prev){
        fvec8 adj_lam(-lambda);
        const float regularzing_weight_const = 1 - lambda * reg_param;
        for(int k = 0; k < prev->size; k++){
            block_vec t_p_act(&prev->activs[k*BATCH_SIZE]);
            for(int j = 0; j < size; j++){
                float & this_weight = weights[k+j * prev->size];
                this_weight *= regularzing_weight_const;
                block_vec t_err(&error[j*BATCH_SIZE]);

                block_vec w_add = (t_err * adj_lam) * t_p_act;
                this_weight += w_add.sum();
            }
        }
        for(int k = 0;k < size; k++)
            biases[k] += (block_vec(&error[k*BATCH_SIZE]) * adj_lam).sum();
    }
};
class batched_output_layer:
    public output_layer_template<batched_layer>
{
public:
    batched_output_layer(int size,int prev_size,float in_lambda,float in_reg_param):
        output_layer_template<batched_layer>(size,prev_size,in_lambda,in_reg_param)
    {}
    batched_output_layer()=default;
    virtual ~batched_output_layer()=default;

    virtual test_info get_correct(vector<float> & expected_activs){
        test_info sum{0,0};
        for(int i = 0; i < BATCH_SIZE; i++){
            float max_val = -1000000.0;
            int max_num = -1;
            int expected_num = -1;
            float sum_diffs_sqrd = 0;
            for(int j = 0; j < size;j++){
                float this_val = activs[j*BATCH_SIZE+i];
                float exp_val = expected_activs[j*BATCH_SIZE+i];
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

            sum += test_info{tot_cost,is_accurate};
        }
        return sum;
    }

    virtual void back_prop_output(vector<float> & expected_activs){
        for(int j = 0; j < size; j++){
            //float is_correct = expected[j];
            //float act_deriv = activ_fn_deriv(zs[j]);
            //float cost_d = cost_deriv(activations[j],is_correct);
            for(int i = 0; i < BATCH_SIZE; i++){
                int n = BATCH_SIZE*j+i;
                float err = activs[n]-expected_activs[n];//cost_d * act_deriv;
                if (err != err)
                    cout <<"arg!";
                error[n] = err;
            }
        }
    }
};


using layer = batched_layer;
using output_layer = batched_output_layer;

//using layer = normal_layer;
//using output_layer =normal_output_layer;
