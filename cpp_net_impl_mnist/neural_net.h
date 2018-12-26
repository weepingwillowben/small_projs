#include "layer.h"
struct learn_data{
    vector<float> input_data;
    vector<float> output_data;
};
class basic_network
{
public:
    basic_network(deque<int> sizes,float in_lambda,float in_reg_param)
    {
        input = layer(sizes[0],0,in_lambda,in_reg_param);
        for(int j = 1;j < sizes.size()-1;j++){
            hidden.emplace_back(sizes[j],sizes[j-1],in_lambda,in_reg_param);
        }
        output = output_layer(sizes.back(),sizes[sizes.size()-2],in_lambda,in_reg_param);
    }
    ~basic_network()=default;
    layer input;
    vector<layer> hidden;
    output_layer output;
    template<typename gen_type>
    void learn(gen_type generator){
        float start_t = GetCounter();
        learn_data accum{vector<float>(input.size*BATCH_SIZE),vector<float>(output.size*BATCH_SIZE)};
        int train_size = 0;
        generator([&](learn_data & data){
            int off = train_size % BATCH_SIZE;
            place(accum,data,off);
            train_size++;
            if(off == 0 && train_size != 1)
                learn_from(accum);
        });
        float tot_time = GetCounter() - start_t;
        cout << "time taken = " << tot_time << "seconds\n";
    }
    template<typename gen_type>
    test_info test(gen_type generator){
        test_info tot{0,0};
        learn_data accum{vector<float>(input.size*BATCH_SIZE),vector<float>(output.size*BATCH_SIZE)};
        int test_size = 0;
        generator([&](learn_data & data){
            int off = test_size % BATCH_SIZE;
            place(accum,data,off);
            test_size++;
            if(off == 0 && test_size != 1)
                tot += test_with(accum);
        });
        cout << "epoc is complete.\n";
        cout << "average cost = " << tot.tot_cost / test_size << "%\n";
        cout << "% of test data accurate = " << float(tot.tot_terms * 100) / test_size << "\n";
        return tot;
    }
    void learn_from(learn_data data){
        copy(data.input_data.begin(),data.input_data.end(),input.activs.begin());
        activate();
        output.back_prop_output(data.output_data);
        grad_descent();
    }
    void place(vector<float> & accum,vector<float> & add,int off){
        for(int n = 0; n < add.size();n++)
            accum[n*BATCH_SIZE+off] = add[n];
    }
    void place(learn_data & accum,learn_data & add,int off){
        place(accum.input_data,add.input_data,off);
        place(accum.output_data,add.output_data,off);
    }
    test_info test_with(learn_data data){
        copy(data.input_data.begin(),data.input_data.end(),input.activs.begin());

        activate();
        return output.get_correct(data.output_data);
    }
    vector<float> get_idea(vector<float> & out_data){
        fill(input.activs.begin(),input.activs.end(),0);

        activate();
        output.back_prop_output(out_data);
        grad_descent();
        vector<float> out;
        for(int j = 0; j < input.error.size(); j+= BATCH_SIZE)
            out.push_back(activ_fn(input.error[j]));
        return out;
    }
    void activate(){
        layer * prev = &input;
        for (layer & lay : hidden){
            lay.activate(prev);
            prev = &lay;
        }
        output.activate(prev);
    }
    void grad_descent(){
        layer * prev = &hidden.back();
        output.gradient_descent(prev);
        vector<layer *> lays;
        lays.push_back(&input);
        for (layer & lay : hidden)
            lays.push_back(&lay);
        lays.push_back(&output);
        for(int l = hidden.size()-1;l > 0;l--){
            int lay_n = l+1;
            hidden[l].gradient_descent(lays[lay_n-1]);
        }
        hidden[0].gradient_descent_input(&input);
    }
};
