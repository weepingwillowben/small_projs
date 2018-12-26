#include "cl_layer.h"
#include "neural_net.h"

struct learn_data{
    vector<float> input_data;
    vector<float> output_data;
};
class cl_network:
        public basic_network
{
public:
    cl_network(deque<int> sizes,float in_lambda,float in_reg_param):
        basic_network(sizes,in_lambda,in_reg_param)
    {
        build_program();
        input = layer(sizes[0],0,in_lambda,in_reg_param,context);
        for(int j = 1;j < sizes.size()-1;j++){
            hidden.emplace_back(sizes[j],sizes[j-1],in_lambda,in_reg_param,context);
        }
        output = output_layer(sizes.back(),sizes[sizes.size()-2],in_lambda,in_reg_param,context);

        input.init(nullptr,program,queue);
        layer * prev = &input;
        for (layer & lay : hidden){
            lay.init(prev,program,queue);
            prev = &lay;
        }
        output.init(prev,program,queue);
    }
    ~cl_network()=default;

    layer input;
    vector<layer> hidden;
    output_layer output;

    //opencl stuff
    cl::Platform platform;
    cl::Device device;
    cl::Context context;
    cl::Program::Sources sources;
    cl::Program program;
    cl::CommandQueue queue;

    template<typename gen_type>
    void learn(gen_type generator){
        float start_t = GetCounter();
        generator([&](learn_data & data){
            learn_from(data);
        });
        float tot_time = GetCounter() - start_t;
        cout << "time taken = " << tot_time << "seconds\n";
    }
    void load_input(learn_data & data){
        handle_error(queue.enqueueWriteBuffer(input.activs,CL_TRUE,0,input.size*sizeof(float),data.input_data.data()));
    }

    void learn_from(learn_data & data){
        load_input(data);

        layer * prev = &input;
        for (layer & lay : hidden){
            lay.activate(prev,queue);
            prev = &lay;
        }
        output.activate(prev,queue);
        output.back_prop_output(data.output_data,queue);
        output.gradient_descent(prev,queue);
        vector<layer *> lays;
        lays.push_back(&input);
        for (layer & lay : hidden)
            lays.push_back(&lay);
        lays.push_back(&output);
        for(int l = hidden.size()-1;l >= 0;l--){
            int lay_n = l+1;
            hidden[l].gradient_descent(lays[lay_n-1],queue);
        }
    }

    template<typename gen_type>
    test_info test(gen_type generator){
        test_info tot{0,0};
        int test_size = 0;
        generator([&](learn_data & data){
            test_size++;
            tot += test_with(data);
        });
        cout << "epoc is complete.\n";
        cout << "average cost = " << tot.tot_cost / test_size << "%\n";
        cout << "% of test data accurate = " << float(tot.tot_terms * 100) / test_size << "\n";
        return tot;
    }
    test_info test_with(learn_data & data){
        load_input(data);
        layer * prev = &input;

        for (layer & lay : hidden){
            lay.activate(prev,queue);
            prev = &lay;
        }
        output.activate(prev,queue);
        return output.get_correct(data.output_data,queue);
    }
    void BuildSource(){
        std::ifstream file("C:/Users/Benjamin/Documents/PythonProjects/neural_nets/net_prac/compute.cl");
        if(!file){
            std::cout << "the file compute.cl is missing!\n";
            exit(1);
        }
        //slow way to read a file (but file size is small)
        std::string fstr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        file.close();

        sources.clear();
        sources.push_back(std::make_pair(fstr.c_str(),fstr.length()));
    }
    void get_main_device(){
        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);
        if(all_platforms.size()==0){
            std::cout<<" No platforms found. Check OpenCL installation!\n";
            exit(1);
        }
        platform=all_platforms[0];
        std::cout << "Using platform: "<<platform.getInfo<CL_PLATFORM_NAME>()<<"\n";

        //get default device of the default platform
        std::vector<cl::Device> all_devices;
        //todo: change CL_DEVICE_TYPE_ALL to something that specifies GPU or accelerator
        cl_int err = platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
        if(err){
            std::cout<<" No devices found. Check OpenCL installation!\n";
            exit(1);
        }
        device=all_devices[0];
        std::cout<< "Using device: "<<device.getInfo<CL_DEVICE_NAME>()<<"\n";
    }
    void build_program(){
        get_main_device();
        context = cl::Context({device});

        queue = cl::CommandQueue(context,device);

        BuildSource();

        program = cl::Program(context,sources);
        if(program.build({device})!=CL_SUCCESS){
            std::cout<<" Error building: "<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device)<<"\n";
            exit(1);
        }
    }
};
