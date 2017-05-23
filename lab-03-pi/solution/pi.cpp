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

int CL_VECTOR_SIZE = 16;
const int samples = 1E7;
// const int samples = 16384;

int field_width = 20;

void print_header() {
  cout << setw(field_width) << "Method" << setw(field_width) << "pi_estimate"
       << setw(field_width) << "total_time (usec)" << setw(field_width)
       << "cpu_time (usec)" << setw(field_width) << "fpga_time (usec)" << endl;
}

void print_result(string method, float pi_estimate, int total_time,
                  int cpu_time, int accelerator_time) {
  cout << setw(field_width) << method;
  cout << setw(field_width) << pi_estimate;
  cout << setw(field_width) << total_time;
  cout << setw(field_width) << cpu_time;
  cout << setw(field_width) << accelerator_time;
  cout << endl;
}

void generate_random_numbers(vector<float> &X, vector<float> &Y) {
  X.resize(samples);
  Y.resize(samples);

  for (int i = 0; i < samples; i++) {
    X[i] = float(rand()) / RAND_MAX;
    Y[i] = float(rand()) / RAND_MAX;
  }
}

void pi_cpu(vector<float> &X, vector<float> &Y) {
  int count = 0;

  // start the timer
  auto start = high_resolution_clock::now();

  // count how many samples reside in the circle
  float x, y;
  for (int i = 0; i < samples; i++) {
    x = X[i];
    y = Y[i];

    if (x * x + y * y < 1.0)
      count++;
  }

  // end the timer and calculate the execution time
  auto stop = high_resolution_clock::now();
  auto time = duration_cast<microseconds>(stop - start).count();

  float pi_estimate = 4 * float(count) / samples;
  print_result("CPU", pi_estimate, time, time, 0);
}

void pi_initial(vector<float> &h_X, vector<float> &h_Y) {
  vector<int> h_results(samples);

  Context context(CL_DEVICE_TYPE_ALL);
  vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

  // create a command queue
  cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;

  CommandQueue queue(context, devices[0], properties, NULL);
  vector<char> contents = readBinary("pi_initial.xclbin");
  Program::Binaries binaries;
  binaries.push_back(make_pair(contents.data(), contents.size()));

  Program program(context, devices, binaries);

  cl::Kernel pi_kernel(program, "pi_initial");

  // Setup buffers
  size_t size = samples * sizeof(float);
  Buffer d_X(context, CL_MEM_COPY_HOST_PTR, size, h_X.data());
  Buffer d_Y(context, CL_MEM_COPY_HOST_PTR, size, h_Y.data());
  Buffer d_results(context, CL_MEM_WRITE_ONLY, size);

  // timer
  Event launch;

  // Setup and launch kernel
  pi_kernel.setArg(0, d_X);
  pi_kernel.setArg(1, d_Y);
  pi_kernel.setArg(2, d_results);
  queue.enqueueNDRangeKernel(pi_kernel, 0, NDRange(samples), NullRange,
                             NULL, &launch);

  launch.wait();
  ulong g_start = launch.getProfilingInfo<CL_PROFILING_COMMAND_START>();
  ulong g_stop = launch.getProfilingInfo<CL_PROFILING_COMMAND_END>();
  float fpga_time = (g_stop - g_start) * 1E-3f;

  // Retrieve results (sum on the CPU)
  queue.enqueueReadBuffer(d_results, CL_TRUE, 0, size, h_results.data());

  auto start = high_resolution_clock::now();

  int sum = 0;
  for (int i = 0; i < h_results.size(); i++)
    sum += h_results[i];

  auto stop = high_resolution_clock::now();
  auto cpu_time = duration_cast<microseconds>(stop - start).count();

  float estimatedValue = 4.0 * sum / samples;

  print_result("Initial", estimatedValue, cpu_time + fpga_time, cpu_time,
               fpga_time);
}

void pi_vector_types(vector<float> &h_X, vector<float> &h_Y) {
  // To convert the pi_initial implementation we:
  //  1) Modify the kernel to use float16 for input (x,y) and int16 for output
  //     (results)
  //  2) (optional) split input data into x and y values.
  //  3) Update the kernel name in readBinary
  //  4) Reduce the number of kernels (e.g. divide the NDRange) by a factor
  //     of 16.

  int CL_VECTOR_SIZE = 16;

  vector<int> h_results(samples);

  Context context(CL_DEVICE_TYPE_ALL);
  vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

  // create a command queue
  cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;

  CommandQueue queue(context, devices[0], properties, NULL);
  vector<char> contents = readBinary("pi_vector_types.xclbin");
  Program::Binaries binaries;
  binaries.push_back(make_pair(contents.data(), contents.size()));

  Program program(context, devices, binaries);

  cl::Kernel pi_kernel(program, "pi_vector_types");

  // Setup buffers
  size_t size = samples * sizeof(float);
  Buffer d_X(context, CL_MEM_COPY_HOST_PTR, size, h_X.data());
  Buffer d_Y(context, CL_MEM_COPY_HOST_PTR, size, h_Y.data());
  Buffer d_results(context, CL_MEM_WRITE_ONLY, size);

  // timer
  Event launch;

  // Setup and launch kernel
  pi_kernel.setArg(0, d_X);
  pi_kernel.setArg(1, d_Y);
  pi_kernel.setArg(2, d_results);
  queue.enqueueNDRangeKernel(pi_kernel, 0, NDRange(samples / CL_VECTOR_SIZE),
                             NullRange, NULL, &launch);

  launch.wait();
  ulong g_start = launch.getProfilingInfo<CL_PROFILING_COMMAND_START>();
  ulong g_stop = launch.getProfilingInfo<CL_PROFILING_COMMAND_END>();
  float fpga_time = (g_stop - g_start) * 1E-3f;

  // Retrieve results (sum on the CPU)
  queue.enqueueReadBuffer(d_results, CL_TRUE, 0, size, h_results.data());

  auto start = high_resolution_clock::now();

  int sum = 0;
  for (int i = 0; i < h_results.size(); i++)
    sum += h_results[i];

  auto stop = high_resolution_clock::now();
  auto cpu_time = duration_cast<microseconds>(stop - start).count();

  float estimatedValue = 4.0 * sum / samples;

  print_result("Vector", estimatedValue, cpu_time + fpga_time, cpu_time,
               fpga_time);
}

void pi_async_copy(vector<float> &h_X, vector<float> &h_Y) {
  vector<int> h_results(samples);

  Context context(CL_DEVICE_TYPE_ALL);
  vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

  // create a command queue
  cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;

  CommandQueue queue(context, devices[0], properties, NULL);
  vector<char> contents = readBinary("pi_async_copy.xclbin");
  Program::Binaries binaries;
  binaries.push_back(make_pair(contents.data(), contents.size()));

  Program program(context, devices, binaries);

  cl::Kernel pi_kernel(program, "pi_async_copy");

  // Setup buffers
  size_t size = samples * sizeof(float);
  Buffer d_X(context, CL_MEM_COPY_HOST_PTR, size, h_X.data());
  Buffer d_Y(context, CL_MEM_COPY_HOST_PTR, size, h_Y.data());
  Buffer d_results(context, CL_MEM_WRITE_ONLY, samples * sizeof(int));

  // timer
  Event launch;

  // Setup and launch kernel
  pi_kernel.setArg(0, d_X);
  pi_kernel.setArg(1, d_Y);
  pi_kernel.setArg(2, d_results);
  pi_kernel.setArg(3, samples);
  queue.enqueueNDRangeKernel(pi_kernel, 0, NDRange(samples / 16, 1, 1),
                             NDRange(4, 1, 1), NULL, &launch);

  launch.wait();
  ulong g_start = launch.getProfilingInfo<CL_PROFILING_COMMAND_START>();
  ulong g_stop = launch.getProfilingInfo<CL_PROFILING_COMMAND_END>();
  float fpga_time = (g_stop - g_start) * 1E-3f;

  // Retrieve results (sum on the CPU)
  queue.enqueueReadBuffer(d_results, CL_TRUE, 0, size, h_results.data());

  auto start = high_resolution_clock::now();

  int sum = 0;
  for (int i = 0; i < h_results.size(); i++)
    sum += h_results[i];

  auto stop = high_resolution_clock::now();
  auto cpu_time = duration_cast<microseconds>(stop - start).count();

  float estimatedValue = 4.0 * sum / samples;

  print_result("Async Copy", estimatedValue, cpu_time + fpga_time, cpu_time,
               fpga_time);
}

void pi_async_with_reduction(vector<float> &h_X, vector<float> &h_Y) {
  vector<int> h_results(samples / CL_VECTOR_SIZE);

  Context context(CL_DEVICE_TYPE_ALL);
  vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

  // create a command queue
  cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;

  CommandQueue queue(context, devices[0], properties, NULL);
  vector<char> contents = readBinary("pi_async_with_reduction.xclbin");
  Program::Binaries binaries;
  binaries.push_back(make_pair(contents.data(), contents.size()));

  Program program(context, devices, binaries);

  cl::Kernel pi_kernel(program, "pi_async_with_reduction");

  // Setup buffers
  size_t size = samples * sizeof(float);
  Buffer d_X(context, CL_MEM_COPY_HOST_PTR, size, h_X.data());
  Buffer d_Y(context, CL_MEM_COPY_HOST_PTR, size, h_Y.data());
  Buffer d_results(context, CL_MEM_WRITE_ONLY, h_results.size() * sizeof(int));

  // timer
  Event launch;

  // Setup and launch kernel
  pi_kernel.setArg(0, d_X);
  pi_kernel.setArg(1, d_Y);
  pi_kernel.setArg(2, d_results);
  pi_kernel.setArg(3, samples);
  queue.enqueueNDRangeKernel(pi_kernel, 0,
                             NDRange(samples / CL_VECTOR_SIZE, 1, 1),
                             NDRange(4, 1, 1), NULL, &launch);

  launch.wait();
  ulong g_start = launch.getProfilingInfo<CL_PROFILING_COMMAND_START>();
  ulong g_stop = launch.getProfilingInfo<CL_PROFILING_COMMAND_END>();
  float fpga_time = (g_stop - g_start) * 1E-3f;

  // Retrieve results (sum on the CPU)
  queue.enqueueReadBuffer(d_results, CL_TRUE, 0, h_results.size() * sizeof(int),
                          h_results.data());

  auto start = high_resolution_clock::now();

  int sum = 0;
  for (int i = 0; i < h_results.size(); i++)
    sum += h_results[i];

  auto stop = high_resolution_clock::now();
  auto cpu_time = duration_cast<microseconds>(stop - start).count();

  float estimatedValue = 4.0 * sum / samples;

  print_result("Async w/ reduction", estimatedValue, cpu_time + fpga_time,
               cpu_time, fpga_time);
}

void pi_vector_with_reduction(vector<float> &h_X, vector<float> &h_Y) {
  vector<int> h_results(samples / CL_VECTOR_SIZE);

  Context context(CL_DEVICE_TYPE_ALL);
  vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

  // create a command queue
  cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;

  CommandQueue queue(context, devices[0], properties, NULL);
  vector<char> contents = readBinary("pi_vector_with_reduction.xclbin");
  Program::Binaries binaries;
  binaries.push_back(make_pair(contents.data(), contents.size()));

  Program program(context, devices, binaries);

  cl::Kernel pi_kernel(program, "pi_vector_with_reduction");

  // Setup buffers
  size_t size = samples * sizeof(float);
  Buffer d_X(context, CL_MEM_COPY_HOST_PTR, size, h_X.data());
  Buffer d_Y(context, CL_MEM_COPY_HOST_PTR, size, h_Y.data());
  Buffer d_results(context, CL_MEM_WRITE_ONLY, size);

  // timer
  Event launch;

  // Setup and launch kernel
  pi_kernel.setArg(0, d_X);
  pi_kernel.setArg(1, d_Y);
  pi_kernel.setArg(2, d_results);
  queue.enqueueNDRangeKernel(pi_kernel, 0, NDRange(samples / CL_VECTOR_SIZE),
                             NullRange, NULL, &launch);

  launch.wait();
  ulong g_start = launch.getProfilingInfo<CL_PROFILING_COMMAND_START>();
  ulong g_stop = launch.getProfilingInfo<CL_PROFILING_COMMAND_END>();
  float fpga_time = (g_stop - g_start) * 1E-3f;

  // Retrieve results (sum on the CPU)
  queue.enqueueReadBuffer(d_results, CL_TRUE, 0, h_results.size() * sizeof(int),
                          h_results.data());

  auto start = high_resolution_clock::now();

  int sum = 0;
  for (int i = 0; i < h_results.size(); i++)
    sum += h_results[i];

  auto stop = high_resolution_clock::now();
  auto cpu_time = duration_cast<microseconds>(stop - start).count();

  float estimatedValue = 4.0 * sum / samples;

  print_result("Vector w/ reduction", estimatedValue, cpu_time + fpga_time,
               cpu_time, fpga_time);
}

int main() {
  // generate random numbers
  vector<float> h_X(samples);
  vector<float> h_Y(samples);
  generate_random_numbers(h_X, h_Y);

  // print the header
  print_header();

  // run individual methods
  pi_cpu(h_X, h_Y);
  pi_initial(h_X, h_Y);
  pi_vector_types(h_X, h_Y);
  pi_vector_with_reduction(h_X, h_Y);
  pi_async_copy(h_X, h_Y);
  pi_async_with_reduction(h_X, h_Y);

  return 0;
}
