#pragma once
#include <vector>
#include <cassert>
#include <iterator>
#include "utils.h"
#include "physical_engine.h"

using namespace std;

using intvec = vector<int>;

using vecfloatit = typename vector<float>::iterator;


char to_lower(char c){
    return c | 32;
}
char all_letters[] = "abcdefghijklmnopqrstuvwxyz\"\' .,;:-";
constexpr int num_letters = sizeof(all_letters);
const int NO_LETTER = -1;
int letter_index(char c){
    for(int i = 0; i < num_letters; i++){
        if(c == all_letters[i]){
            return i;
        }
    }
    return NO_LETTER;
}
char index_letter(char c){
    return all_letters[c];
}

intvec convert_to_standard(string input){
    intvec res;
    for(char c : input){
        char lowc = to_lower(c);
        int let_idx = letter_index(lowc);
        if (let_idx != NO_LETTER){
            res.push_back(let_idx);
        }
    }
    return res;
}

class Letters:
    public PhyiscalEng{
protected:
    static constexpr size_t wait_count = 1;//5;
    const float expected_mag = 1.0f;
    vector<float> nextinputs;
    vector<float> outputs;
    float learn_val;
    
    int text_counter;
    intvec text;
    
    int wait_counter = 0; 
public:
    Letters(string in_text):
        nextinputs(num_letters),
        outputs(num_letters),
        learn_val(0),
        text_counter(0),
        text(convert_to_standard(in_text))
        {assert(text.size() > 0);}
    
    virtual const vector<float> & new_inputs(){
        return nextinputs;
    }
    virtual float get_learn_val(){
        return learn_val;
    }
    virtual void calculate_values(){
        wait_counter = (wait_counter + 1) % wait_count;
        if (wait_counter == 0){
            go_to_next_value();
        }
        set_learn_val();
    }
    virtual void set_outputs(vecfloatit begin, vecfloatit end){
        assert(distance(begin,end) == num_letters);
        outputs.assign(begin,end);
        print_output();
    }
    virtual int output_size(){
        return num_letters;
    }
    virtual int input_size(){
        return num_letters;
    }
protected:
    void print_output(){
        int outidx = max_idx(outputs);
        cout << index_letter(outidx);
    }
    void go_to_next_value(){
        fill(nextinputs.begin(),nextinputs.end(),0);
        
        int new_counter_idx = (text_counter + 1) % text.size();
        int new_letter = text[new_counter_idx];
        nextinputs[new_letter] = expected_mag;
        text_counter = new_counter_idx;
    }
    void set_learn_val(){
        int cur_text_idx = 15;//text[text_counter];
        float sum_squares = 0;
        for(int i = 0; i < num_letters; i++){
            int expected_num = (cur_text_idx == i);
            float expected = expected_mag*expected_num;
            sum_squares += sqr(outputs[i] - expected);
        }
        learn_val = sum_squares;
        //learn_val = -(-1.0f + 2*(max_idx(outputs) == 15));
    }
};
