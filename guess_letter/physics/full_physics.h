#pragma once
#include <vector>
#include <memory>
#include <cassert>
#include "physical_engine.h"

class FullPhysics:
    public PhyiscalEng{
protected:
    vector<float> nextinputs;
public:
    vector<unique_ptr<PhyiscalEng>> engines;
    
    void add_engine(unique_ptr<PhyiscalEng> eng){
        engines.emplace_back(move(eng));
    }
    
    virtual const vector<float> & new_inputs(){
        nextinputs.resize(input_size());
        vecfloatit start = nextinputs.begin();
        
        int curpos = 0;
        for(const unique_ptr<PhyiscalEng> & eng : engines){
            const vector<float> & curin = eng->new_inputs();
            copy(curin.begin(),curin.end(),start);
            start += curin.size();
            assert(curin.size() == eng->input_size());
        }
        return nextinputs;
    }

    virtual float get_learn_val(){
        float sum = 0;
        for(const unique_ptr<PhyiscalEng> & eng : engines){
            sum += eng->get_learn_val();
        }
        return sum;
    }

    virtual void calculate_values(){
        for(const unique_ptr<PhyiscalEng> & eng : engines){
            eng->calculate_values();
        }
    }

    virtual void set_outputs(vecfloatit begin, vecfloatit end){
        assert(distance(begin,end) == output_size());
        vecfloatit start = begin;
        for(const unique_ptr<PhyiscalEng> & eng : engines){
            
            vecfloatit curend = start + eng->output_size();
            
            eng->set_outputs(start,curend);
            
            start = curend;
        }
    }

    virtual int output_size(){
        int sum = 0;
        for(const unique_ptr<PhyiscalEng> & eng : engines){
            sum += eng->output_size();
        }
        return sum;
    }

    virtual int input_size(){
        int sum = 0;
        for(const unique_ptr<PhyiscalEng> & eng : engines){
            sum += eng->input_size();
        }
        return sum;
    }
};
