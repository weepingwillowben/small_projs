#pragma once
#include <vector>
#include <cassert>
#include "physical_engine.h"
#include "sparse_mat_mul.h"
#include "utils.h"

class SpringState:
        public PhyiscalEng{
protected:
    int size;
    vector<float> inputs;
    vector<float> outputs;
    
    vector<float> state;
    vector<float> reset_val;
    float learn_val;
public:
    SpringState(int my_size):
      size(my_size),
      inputs(my_size),
      outputs(my_size),
      state(my_size),
      reset_val(my_size),
      learn_val(0){}
    
    virtual const vector<float> & new_inputs(){
        return outputs;
    }

    virtual float get_learn_val(){
        return 0;
    }
    
    virtual void calculate_values(){
        /*for(int i = 0; i < size; i++){
            reset_val[i] = reset_val[i] * state[i] >= 0 ? reset_val[i] + sqr(state[i]) : 0;
        }
        in_place_add(inputs,state);
        for(int i = 0; i < size; i++){
            outputs[i] = tanh(state[i] * 0.001/(0.0001+reset_val[i]));
        }*/
        outputs = inputs;
    }

    virtual void set_outputs(vecfloatit begin, vecfloatit end){
        assert(distance(begin,end) == inputs.size());
        inputs.assign(begin,end);
    }

    virtual int output_size(){
        return size;
    }

    virtual int input_size(){
        return size;
    }
};
