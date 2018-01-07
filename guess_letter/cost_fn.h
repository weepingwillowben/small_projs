#pragma once
#include "sparse_mat_mul.h"

class CostFn{
protected:
    vecfloat est_gradients;
public:
    CostFn(int inoutsize):
        est_gradients(inoutsize)
    {}
    const vecfloat & estimate_cost(const vecfloat & outputs,float learn_val){
        assert(size() == outputs.size());
        for(int i = 0; i < size(); i++){
            float deriv = 1;//sigderiv(outputs[i]);
            est_gradients[i] = i >= 30 ? 0 : deriv * learn_val;
        }
        assert_all_non_negative();
        return est_gradients;
    }
    int size(){
        return est_gradients.size();
    }
protected:
    void assert_all_non_negative(){
        for(float v : est_gradients){
            assert(v >= 0 && "Cannot use RMS stablizer on negative costs");
        }
    }
};
