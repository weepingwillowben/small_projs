#include "net_algorithms.hpp"
#include "CL/cl.hpp"
std::string get_error_string(cl_int err){
     switch(err){
         case 0: return "CL_SUCCESS";
         case -1: return "CL_DEVICE_NOT_FOUND";
         case -2: return "CL_DEVICE_NOT_AVAILABLE";
         case -3: return "CL_COMPILER_NOT_AVAILABLE";
         case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
         case -5: return "CL_OUT_OF_RESOURCES";
         case -6: return "CL_OUT_OF_HOST_MEMORY";
         case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
         case -8: return "CL_MEM_COPY_OVERLAP";
         case -9: return "CL_IMAGE_FORMAT_MISMATCH";
         case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
         case -11: return "CL_BUILD_PROGRAM_FAILURE";
         case -12: return "CL_MAP_FAILURE";

         case -30: return "CL_INVALID_VALUE";
         case -31: return "CL_INVALID_DEVICE_TYPE";
         case -32: return "CL_INVALID_PLATFORM";
         case -33: return "CL_INVALID_DEVICE";
         case -34: return "CL_INVALID_CONTEXT";
         case -35: return "CL_INVALID_QUEUE_PROPERTIES";
         case -36: return "CL_INVALID_COMMAND_QUEUE";
         case -37: return "CL_INVALID_HOST_PTR";
         case -38: return "CL_INVALID_MEM_OBJECT";
         case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
         case -40: return "CL_INVALID_IMAGE_SIZE";
         case -41: return "CL_INVALID_SAMPLER";
         case -42: return "CL_INVALID_BINARY";
         case -43: return "CL_INVALID_BUILD_OPTIONS";
         case -44: return "CL_INVALID_PROGRAM";
         case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
         case -46: return "CL_INVALID_KERNEL_NAME";
         case -47: return "CL_INVALID_KERNEL_DEFINITION";
         case -48: return "CL_INVALID_KERNEL";
         case -49: return "CL_INVALID_ARG_INDEX";
         case -50: return "CL_INVALID_ARG_VALUE";
         case -51: return "CL_INVALID_ARG_SIZE";
         case -52: return "CL_INVALID_KERNEL_ARGS";
         case -53: return "CL_INVALID_WORK_DIMENSION";
         case -54: return "CL_INVALID_WORK_GROUP_SIZE";
         case -55: return "CL_INVALID_WORK_ITEM_SIZE";
         case -56: return "CL_INVALID_GLOBAL_OFFSET";
         case -57: return "CL_INVALID_EVENT_WAIT_LIST";
         case -58: return "CL_INVALID_EVENT";
         case -59: return "CL_INVALID_OPERATION";
         case -60: return "CL_INVALID_GL_OBJECT";
         case -61: return "CL_INVALID_BUFFER_SIZE";
         case -62: return "CL_INVALID_MIP_LEVEL";
         case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
         default: return "Unknown OpenCL error";
     }
 }
void handle_error(cl_int err){
    if (err){
        std::cout << "Error: " << get_error_string(err) << "\n";
    }
}


template <int n>
void set_kern_args(cl::Kernel & kernel){}

template <int n,typename this_arg,typename ... args>
void set_kern_args(cl::Kernel & kernel,this_arg & arg,args & ... argsw){
    //this implementation will set the last
    kernel.setArg(n,arg);
    set_kern_args<n+1,args...>(kernel,argsw...);
}
void read(cl::CommandQueue & queue,cl::Buffer & src,vector<float> & dest){

}

class layer{
public:
    layer()=default;
    layer(int in_size,int in_prev_size,float in_lambda,float in_reg_param,cl::Context & context):
            lambda(in_lambda),
            reg_param(in_reg_param),
            size(in_size),
            biases(context,CL_MEM_READ_WRITE,size*sizeof(float)),
            weights(context,CL_MEM_READ_WRITE,size*in_prev_size*sizeof(float)),
            activs(context,CL_MEM_READ_WRITE,size*sizeof(float)),
            zs(context,CL_MEM_READ_WRITE,size*sizeof(float)),
            error(context,CL_MEM_READ_WRITE,size*sizeof(float))
    {}

     ~layer() = default;
    float lambda;
    float reg_param;
    int size;

    cl::Buffer biases;
    cl::Buffer weights;
    cl::Buffer activs;
    cl::Buffer zs;
    cl::Buffer error;

    cl::Kernel activate_kern;
    cl::Kernel zprime_kern;
    cl::Kernel grad_des_kern;
    cl::Kernel change_bias;

    void init(layer * prev,cl::Program & program,cl::CommandQueue & queue){
        if (prev == nullptr)
            return;
        vector<float> bias_vec(size);
        for(float & f : bias_vec)
            f = rand_bias();
        vector<float> weight_vec(size*prev->size);
        for(float & w : weight_vec)
            w = rand_weight(prev->size);

        queue.enqueueWriteBuffer(biases,CL_TRUE,0,size*sizeof(float),bias_vec.data());

        queue.enqueueWriteBuffer(weights,CL_TRUE,0,size*prev->size*sizeof(float),weight_vec.data());

        activate_kern = cl::Kernel(program,"activate");
        activate_kern.setArg(0,prev->activs);
        activate_kern.setArg(1,weights);
        activate_kern.setArg(2,biases);
        activate_kern.setArg(3,zs);
        activate_kern.setArg(4,activs);
        activate_kern.setArg(5,prev->size);

        grad_des_kern = cl::Kernel(program,"gradient_descent_weights");
        grad_des_kern.setArg(0,weights);
        grad_des_kern.setArg(1,prev->zs);
        grad_des_kern.setArg(2,prev->activs);
        grad_des_kern.setArg(3,error);
        grad_des_kern.setArg(4,prev->error);
        grad_des_kern.setArg(5,lambda/size);
        grad_des_kern.setArg(6,1-reg_param);
        grad_des_kern.setArg(7,size);
        grad_des_kern.setArg(8,prev->size);

        change_bias = cl::Kernel(program,"add_bias_error");
        change_bias.setArg(0,biases);
        change_bias.setArg(1,error);
        change_bias.setArg(2,lambda);
    }
    void activate(layer * prev,cl::CommandQueue & queue){
        queue.enqueueNDRangeKernel(activate_kern,cl::NullRange,cl::NDRange(size),cl::NullRange);
    }
    void gradient_descent(layer * prev,cl::CommandQueue & queue){
        //initializes error
        queue.enqueueNDRangeKernel(grad_des_kern,cl::NullRange,cl::NDRange(prev->size),cl::NullRange);
        queue.enqueueNDRangeKernel(change_bias,cl::NullRange,cl::NDRange(size),cl::NullRange);
    }
};

class output_layer:
    public layer
{
public:
    output_layer(int size,int prev_size,float in_lambda,float in_reg_param,cl::Context & context):
        layer(size,prev_size,in_lambda,in_reg_param,context),
        expected(context,CL_MEM_READ_WRITE,size*sizeof(float))
    {}
    output_layer()=default;

    cl::Buffer expected;
    cl::Kernel set_error;
    void init(layer * prev,cl::Program & program,cl::CommandQueue & queue){
        layer::init(prev,program,queue);

        set_error = cl::Kernel(program,"set_output_error");
        set_error.setArg(0,error);
        set_error.setArg(1,activs);
        set_error.setArg(2,expected);
    }

    test_info get_correct(vector<float> & expected_activs,cl::CommandQueue & queue){
        vector<float> this_activs(size);
        //copy data from gpu
        queue.enqueueReadBuffer(activs,CL_TRUE,0,size * sizeof(float),this_activs.data());

        float max_val = -1000000.0;
        int max_num = -1;
        int expected_num = -1;
        float sum_diffs_sqrd = 0;
        for(int j = 0; j < size;j++){
            float this_val = this_activs[j];
            float exp_val = expected_activs[j];
            sum_diffs_sqrd += sqare(exp_val - this_val);
            if(this_val > max_val){
                max_val = this_val;
                max_num = j;
            }
            if (exp_val == float(1))
                expected_num = j;
        }
        if(max_num == -1 || expected_num == -1)
            cout << "error!\n";

        float tot_cost = sqrt(sum_diffs_sqrd);
        bool is_accurate = expected_num == max_num;

        return test_info{tot_cost,is_accurate};
    }
    void back_prop_output(vector<float> & expected_activs,cl::CommandQueue & queue){
        //copies data to gpu
        queue.enqueueWriteBuffer(expected,CL_TRUE,0,size*sizeof(float),expected_activs.data());

        queue.enqueueNDRangeKernel(set_error,cl::NullRange,cl::NDRange(size),cl::NullRange);
    }
};
