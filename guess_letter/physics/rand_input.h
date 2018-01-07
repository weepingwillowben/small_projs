#pragma once
#include <vector>
#include <cassert>
#include <random>
#include "physical_engine.h"
#include "sparse_mat_mul.h"
#include "utils.h"

class RandInputs:
        public PhyiscalEng{
protected:
    int size;
    vector<float> rand_vals;
public:
    RandInputs(int my_size):
      rand_vals(my_size){}
    
    virtual const vector<float> & new_inputs(){
        return rand_vals;
    }

    virtual float get_learn_val(){
        return 0;
    }
    
    virtual void calculate_values(){
        for(float & v : rand_vals){
            v = nrand();
        }
    }

    virtual void set_outputs(vecfloatit begin, vecfloatit end){}

    virtual int output_size(){
        return 0;
    }

    virtual int input_size(){
        return rand_vals.size();
    }
};
