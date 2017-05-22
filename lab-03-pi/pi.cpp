/*
  ArrayFire Training Day 1
  Lab: Monte Carlo Pi Estimation

  Description:
  This program will estimate pi by calculating the ratio of	points that fall
  inside of a unit circle with the points that did not
*/

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include <cl_helpers.hpp>

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::microseconds;
using std::cout;
using std::endl;
using std::make_pair;
using std::setw;
using std::string;
using std::vector;

using cl::Buffer;
using cl::CommandQueue;
using cl::Context;
using cl::Device;
using cl::Event;
using cl::Kernel;
using cl::NDRange;
using cl::NullRange;
using cl::Program;

const int samples = 1E7;
int field_width = 20;

void print_header()
{
    cout << setw(field_width) << "Method"
        << setw(field_width) <<  "pi_estimate"
        << setw(field_width) <<  "total_time (usec)"
        << setw(field_width) <<  "cpu_time (usec)"
        << setw(field_width) <<  "fpga_time (usec)"
        << endl;
}

void print_result(string method, float pi_estimate, int total_time,
	int cpu_time, int accelerator_time)
{
    cout << setw(field_width) << method;
    cout << setw(field_width) << pi_estimate;
    cout << setw(field_width) << total_time;
    cout << setw(field_width) << cpu_time;
    cout << setw(field_width) << accelerator_time;
    cout << endl;
}


void pi_cpu(void)
{
    int count = 0;
    float x, y;
    vector<float> randomNums(2*samples);

    // init random numbers
    for(int i = 0; i < 2*samples; i++)	{
      randomNums[i] = float(rand()) / RAND_MAX;
    }

    // start the timer
    auto start = high_resolution_clock::now();

    // count how many samples reside in the circle
    for(int i = 0; i < samples; i++)
    {
      x = randomNums[2*i];
      y = randomNums[2*i + 1];

      if(x*x + y*y < 1.0)
        count++;
    }

    // end the timer and calculate the execution time
    auto stop = high_resolution_clock::now();
    auto time = duration_cast<microseconds>(stop - start).count();

    float pi_estimate = 4 * float(count) / samples;
    print_result("CPU", pi_estimate, time, time, 0);
}


void pi_initial()
{
    float cpu_time = 0;
    float fpga_time = 0;

    vector<float> h_randNums(2*samples);
    vector<float> h_results(samples);

    srand(time(NULL));

    for(int i = 0; i < 2*samples; i++)
    {
      h_randNums[i] = float(rand()) / RAND_MAX;
    }

    Context context(CL_DEVICE_TYPE_ALL);
    vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

    // create a command queue
    cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;
    CommandQueue queue(context, devices[0], properties, NULL);

    std::vector<char> contents = readBinary("pi_initial.xclbin");
    Program::Binaries binaries;
    binaries.push_back(std::make_pair(contents.data(), contents.size()));

    Program program(context, devices, binaries);

    vector<Kernel> kernels;
    program.createKernels(&kernels);

	  // Setup buffers

    // Setup and launch kernel

    // Retrieve results (sum on the CPU)
    int sum = 0;
    for(auto value: h_results)
      sum += value;

    // Compute the estimate for pi and print the result
    float estimatedValue = 4.0 * sum / samples;
    print_result("Initial", estimatedValue, cpu_time + fpga_time, cpu_time, fpga_time);
}


int main()
{
	pi_cpu();
	pi_initial();

	return 0;
}
