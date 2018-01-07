#include <iostream>
#include "read_file.h"
#include "state_map.h"
#include "physical_engine.h"
#include "physics/full_physics.h"
#include "physics/letters.h"
#include "physics/state_manip.h"
#include "physics/rand_input.h"
#include "coststabilzer.h"

using namespace std;

constexpr int intermed_len = 0;
constexpr int num_connections = 30;
constexpr int rand_len = 5;

constexpr float decay_rate = 0.9;
constexpr float learn_val = 0.01;

string read_text(){
    return read_file("../guess_letter/huck_fin.txt");
}


void run_main(){
    string source = read_text();
    //StateMap map(0,0,5000,100);
    
    FullPhysics all;
    all.add_engine(unique_ptr<PhyiscalEng>(new Letters(source)));
    all.add_engine(unique_ptr<PhyiscalEng>(new RandInputs(rand_len)));
    all.add_engine(unique_ptr<PhyiscalEng>(new SpringState(intermed_len)));
    
    StateMap map(all.input_size(),all.output_size(),num_connections);
    
    CostFn cost_fn(all.output_size());
    RMS_Stablizer cost_stablizer(all.output_size(),decay_rate,learn_val);
    //RMS_Stablizer cost_stablizer(all.output_size(),decay_rate,learn_val);
    
    vector<float> true_outputs(all.output_size());
    for(int i = 0; i < 200000000; i++){
        //calc forward
        sparse_mat_mul(map.outs_vals,map.nodes,map.ins_vals);
        //apply_vec(map.outs_vals,sigmoid);
        
        //back prop
        const vecfloat & raw_costs = cost_fn.estimate_cost(map.outs_vals,all.get_learn_val());
        cost_stablizer.add_next_raw_costs(raw_costs);
        const vecfloat & stable_costs = cost_stablizer.stable_vals();
        update_edges(map.nodes,map.ins_vals,stable_costs);
        
        map.get_output(true_outputs);
        all.set_outputs(true_outputs.begin(),true_outputs.end());
        all.calculate_values();
        map.set_input(all.new_inputs());
        //cout << "diff: " << map.diff_avg_out() << "\t\tmag: " << map.get_magnitude_output() << "\t\tlearnval: " << all.get_learn_val() << endl;
    }
}

int main(int argc, char *argv[])
{
    rand_gen.seed(clock());
    run_main();
    cout << "Hello World!" << endl;
    return 0;
}
