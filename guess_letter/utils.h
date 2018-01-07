#pragma once
#include <vector>
#include <cassert>

using namespace std;

int max_idx(const vector<float> & vals){
    assert(vals.size() > 0 && "cannot take max of zero lenght vector");
    float maxval = vals[0];
    int maxidx = 0;
    for(int i = 0; i < vals.size(); i++){
        if(maxval < vals[i]){
            maxval = vals[i];
            maxidx = i;
        }
    }
    return maxidx;
}
