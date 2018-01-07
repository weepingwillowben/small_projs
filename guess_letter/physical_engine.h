#pragma once
#include <vector>

using namespace std;

using vecfloatit = typename vector<float>::iterator;

class PhyiscalEng{
public:
    virtual const vector<float> & new_inputs() = 0;
    virtual float get_learn_val() = 0;
    virtual void calculate_values() = 0;
    virtual void set_outputs(vecfloatit begin, vecfloatit end) = 0;
    virtual int output_size() = 0;
    virtual int input_size() = 0;
};
