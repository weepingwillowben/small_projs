#pragma once
#include "sparse_mat_mul.h"


class AbstractStablizer{
protected:
    vecfloat stabl_vals;
public:
    AbstractStablizer(int in_size):
        stabl_vals(in_size)
        {}

    const vecfloat & stable_vals(){
        return stabl_vals;
    }
    virtual void add_next_raw_costs(const vecfloat & invals) = 0;
protected:
    int size() const {
        return stabl_vals.size();
    }
};

class RMS_Stablizer:
    public AbstractStablizer{
protected:
    float mean_sqr_average;
    
    //
    float decay_rate;
    float learn_val;
public:
    RMS_Stablizer(int in_size, float in_decay_rate, float in_learn_val, float ms_av_start = 100.0):
        AbstractStablizer(in_size),
        mean_sqr_average(ms_av_start),
        decay_rate(in_decay_rate),
        learn_val(in_learn_val)
        {}
    void add_next_raw_costs(const vecfloat & invals){
        assert(stabl_vals.size() == invals.size());
        copy(invals.begin(),invals.end(),stabl_vals.begin());
        
        mean_sqr_average = decay_rate * mean_sqr_average + (1 - decay_rate) * get_mean_sqr();
        
        float mul_val = learn_val / (0.0001f + sqrt(mean_sqr_average));
        for(float & v : stabl_vals){
            v *= mul_val;
        }
    }
protected:
    float get_mean_sqr(){
        float sum = 0;
        for(float v : stabl_vals){
            sum += sqr(v);
        }
        return sum / size();
    }
};
