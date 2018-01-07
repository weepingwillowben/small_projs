#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <CL/cl.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "errorhandling.hpp"
#include <random>
#include <vector>
#include "mat_mult.h"
#include "shared.h"

using namespace std;

const int num_iters = 8;
const int VEC_SIZE = MAT_SIZE * MAT_SIZE;
const int buffer_size = VEC_SIZE * sizeof(float);

void handle_error(cl_int err);

void BuildSource(cl::Program::Sources & sources);
std::vector<float> rand_vec(int size){
    std::default_random_engine engine(clock());
    std::uniform_real_distribution<float> dist;
    std::vector<float> vec(size);
    for(int i = 0; i < size; i++){
        vec[i] = dist(engine);
    }
    return vec;
}
bool are_same(float a,float b){
    return abs(a - b) < 0.01;
}

bool are_same(vector<float> & v1,vector<float> & v2){
    if(v1.size() != v2.size()){
        return false;
    }
    for(size_t i = 0; i < v1.size(); i++){
        if(!are_same(v1[i],v2[i])){
            cout << i << endl << v1[i] << endl << v2[i] << endl;
            return false;
        }
    }
    return true;
}
void print_vec(vector<float> & vec){
    for(int i = 0; i < VEC_SIZE; i++){
        cout << vec[i] << "\n";
    }
    cout << endl;
}
int setenv(const char *name, const char *value)
{
    return _putenv_s(name, value);
}
int main(){
    //disables NVIDIA caches
    setenv("CUDA_CACHE_DISABLE", "1");
    //get all platforms (drivers)
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if(all_platforms.size()==0){
        std::cout<<" No platforms found. Check OpenCL installation!\n";
        exit(1);
    }
    cl::Platform default_platform=all_platforms[0];
    std::cout << "Using platform: "<<default_platform.getInfo<CL_PLATFORM_NAME>()<<"\n";

    //get default device of the default platform
    std::vector<cl::Device> all_devices;
    cl_int err = default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if(err){
        std::cout<<" No devices found. Check OpenCL installation!\n";
        exit(1);
    }
    cl::Device default_device=all_devices[1];
    std::cout<< "Using device: "<<default_device.getInfo<CL_DEVICE_NAME>()<<"\n";


    cl::Context context({default_device});

    cl::Program::Sources sources;
    BuildSource(sources);

    cl::Program program(context,sources);
    if(program.build({default_device},"-cl-fast-relaxed-math")!=CL_SUCCESS){
        std::cout <<" Error building: "<< "\n " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
        exit(1);
    }
    // create buffers on the device
    cl::Buffer gpubuf1 = cl::Buffer(context,CL_MEM_READ_WRITE,buffer_size);
    cl::Buffer gpubuf2 = cl::Buffer(context,CL_MEM_READ_WRITE,buffer_size);
    cl::Buffer gpubufres = cl::Buffer(context,CL_MEM_READ_WRITE,buffer_size);

    //creates the kernel
    cl::Kernel gpu_mat_mult = cl::Kernel(program,"mat_mult");
    cl::Kernel zero_buffer = cl::Kernel(program,"zero_buffer");

    //create queue to which we will push commands for the device.
    cl::CommandQueue queue(context,default_device);

    std::vector<float> mat1 = rand_vec(VEC_SIZE);
    std::vector<float> mat2 = rand_vec(VEC_SIZE);
    std::vector<float> cpumatres(VEC_SIZE);
    std::vector<float> gpumatres(VEC_SIZE);
    int start_cpu = clock();
    
    for(int i = 0; i < num_iters; i++){
    cpu_mat_mult(mat1,mat2,cpumatres);
    }

    int tot_cpu = clock() - start_cpu;
    cout << "Total CPU time = " << tot_cpu << endl;

    int start_gpu = clock();
    queue.enqueueWriteBuffer(gpubuf1,CL_TRUE,0,buffer_size,mat1.data());
    queue.enqueueWriteBuffer(gpubuf2,CL_TRUE,0,buffer_size,mat2.data());
    gpu_mat_mult.setArg(0,gpubuf1);
    gpu_mat_mult.setArg(1,gpubuf2);
    gpu_mat_mult.setArg(2,gpubufres);

    int gpu_compute_time_start = clock();
    for(int i = 0; i < num_iters; i++){
        err = queue.enqueueNDRangeKernel(gpu_mat_mult,cl::NullRange,cl::NDRange(MAT_SIZE,MAT_SIZE/4),cl::NullRange);
        
        queue.enqueueReadBuffer(gpubufres,CL_TRUE,0,buffer_size,gpumatres.data());
        
        handle_error(err);
    }

    int tot_gpu_compute_time = clock() - gpu_compute_time_start;


    int tot_gpu = clock() - start_gpu;
    cout << "Total GPU time = " << tot_gpu << endl;
    cout << "Overhead GPU time = " << tot_gpu - tot_gpu_compute_time << endl;
    cout << "Compute GPU time = " << tot_gpu_compute_time << endl;

    bool vecs_are_same = are_same(gpumatres,cpumatres);
    cout << (vecs_are_same ? "Results are the same" : "FAILED RESULTS NOT THE SAME") << endl;
    if(!vecs_are_same){
        for(int i = 0; i < MAT_SIZE; i++){
            //cout << gpumatres[i] << "\t\t" << cpumatres[i] << "\t\t";
            if(!are_same(gpumatres[i],cpumatres[i])){
                cout << "failed\n";
            }
           // cout << "\n";
        }
    }

    //alternative way to run the kernel
    //cl::KernelFunctor count_prime(cl::Kernel(program,"count_prime"),queue,cl::NullRange,cl::NDRange(num_of_execs),cl::NullRange);
    //count_prime(Counts);
    return 0;
}
void BuildSource(cl::Program::Sources & sources){
    std::ifstream file("C:/Users/benblack/Documents/PythonProjects/cl_prac/opencl_prac/prac.cl");
    if(!file){
        std::cout << "the file prac.cl is missing!\n";
        throw 1;
    }
    std::string fstr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    sources.clear();
    sources.push_back(std::make_pair(fstr.c_str(),fstr.length()));
}
void handle_error(cl_int err){
    if (err){
        std::cout << "Error: " << get_error_string(err) << "\n";
    }
}
