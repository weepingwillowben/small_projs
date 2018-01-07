#pragma once
#include <vector>
#include <cassert>
#include <ctime>
#include "sparse_mat_mul.h"
#include "cost_fn.h"

using namespace std;

/*
 * The goal here is this: A learner which has the following properties:
 * 
 * 1. Approximates a linear learner (or, similarly, learns approximately as well as a perceptron)
 * 2. Input costs make no sense absolutely, but are guarenteed to make sense relatively (additively?) 
 *        in terms of both time and space.
 * 3. Input magnitude is unpredictable, but output magnitude must be stable. 
*/


class StateMap{
public:
    const float avg_decay_rate = 0.9;
    
    int in_len;
    int out_len;
    int num_connections_per;
    //to keep indicies consistent, data is stored like:
    //intermed||input
    //outputs are like
    //intermed||output
    vector<vector<Edge>> nodes;
    vector<float> ins_vals;
    
    vector<float> outs_vals;
    vector<float> outs_averages;
    //vector<float> intermed_strength;
    
    StateMap(int in_in_len,int in_out_len,int in_num_connections_per):
        in_len(in_in_len),
        out_len(in_out_len),
        num_connections_per(in_num_connections_per),
        nodes(in_len),
        ins_vals(in_len),
        outs_vals(out_len),
        outs_averages(out_len)
    {
        assert(num_connections_per < out_len - 1 && "cannot make connections to more nodes than there exist");
        
        rand_init_ins();
        rand_gen_nodes();
    }
    
    void set_input(const vector<float> & true_inputs){
        assert(true_inputs.size() == in_len);
        copy(true_inputs.begin(),true_inputs.end(),ins_vals.begin());
    }
    void get_output(vector<float> & true_outputs){
        for(int i = 0; i < out_len; i++){
            float true_out = outs_vals[i];// - outs_averages[i];
            true_outputs[i] = true_out;
        }
    }
    void adjust_avg(){
        assert(outs_vals.size() == outs_averages.size());
        for(int i = 0; i < outs_vals.size();i++){
            outs_averages[i] = avg_decay_rate * outs_averages[i] + (1 - avg_decay_rate) * outs_vals[i];
        }
    }
    float get_magnitude_output(){
        vector<float> temp_outs(out_len);
        sparse_mat_mul(temp_outs,nodes,ins_vals);
        float mag = 0;
        for(float oval : temp_outs){
            mag += abs(oval);
        }
        return mag;
    }

    void calc_step(){
        //adjust_avg();
    }
    void back_prop(float learn_val){
        //degrade_weights(nodes);
        //standardize_input_mags();
    }

    float diff_avg_out(){
        assert(outs_vals.size() == outs_averages.size());
        float tot_diff = 0;
        for(int i = 0; i < outs_vals.size(); i++){
            tot_diff += abs(outs_vals[i] - outs_averages[i]);
        }
        return tot_diff;
    }
    int max_dests_per_node(){
        return 1 + (in_len*num_connections_per - 1) / out_len;
    }
    void rand_init_ins(){
        for(int i = 0; i < in_len; i++){
            ins_vals[i] = abs(nrand());
        }
    }
    void rand_gen_nodes(){
        vector<int> num_dests(out_len);
        
        for(int nodeidx = 0; nodeidx < in_len; nodeidx++){
            nodes[nodeidx].resize(num_connections_per);
            for(int edgei = 0; edgei < num_connections_per; edgei++){
                int con_idx;
                do{
                    con_idx = get_rand_num(out_len);
                }
                while(con_idx == nodeidx || num_dests[con_idx] >= max_dests_per_node());
                
                nodes[nodeidx][edgei] = Edge{con_idx,get_rand_weight(num_connections_per)};
                
                num_dests[con_idx]++;
            }
        }
        sanity_check_rand_connections(num_dests);
        standardize_input_mags();
    }
    void sanity_check_rand_connections(vector<int> num_dests){
        for(int n : num_dests){
            assert(n <= max_dests_per_node());
        }
        for(int nodei = 0; nodei < in_len; nodei++){
            auto & nedges = nodes[nodei];
            assert(nedges.size() == num_connections_per);
            for(Edge & e : nedges){
                assert(e.node_num != nodei);
                assert(e.connection_weight != 0 && e.connection_weight > -10 && e.connection_weight < 10);
            }
        }
    }
    void standardize_input_mags(){
        for(vecEdge & ve : nodes){
            ::standardize_input_mags(ve);
        }
    }
};
