/*
  ArrayFire Training Day 1
  Lab: Vector Addition

  This program will add two vectors and store the result in a third vector
  using the FPGA
*/

#define __CL_ENABLE_EXCEPTIONS

#include "cl_helpers.hpp"
#include <CL/cl.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cl;

int main(void) {
  cl::Context context(CL_DEVICE_TYPE_ALL);
  vector<Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
  for (int i = 0; i < devices.size(); i++) {
    cout << "Device Name: " << devices[i].getInfo<CL_DEVICE_NAME>() << endl;
    ;
  }

  cl::CommandQueue queue(context, devices[0]);
  vector<char> contents = readBinary("vector_addition.xclbin");
  cl::Program::Binaries binaries;
  binaries.push_back(std::make_pair(contents.data(), contents.size()));

  cl::Program add_program(context, devices, binaries);

  cl::Kernel add_kernel(add_program, "add");

  // setup data and create buffers
  int N = 100;
  ::size_t size = N * sizeof(int);
  vector<int> h_a(N, 1);
  vector<int> h_b(N, 2);
  vector<int> h_c(N);

  Buffer d_a(context, CL_MEM_COPY_HOST_PTR, size, &h_a.front());
  Buffer d_b(context, CL_MEM_COPY_HOST_PTR, size, &h_b.front());
  Buffer d_c(context, CL_MEM_WRITE_ONLY, size);

  // Setup and launch kernel
  add_kernel.setArg(0, d_a);
  add_kernel.setArg(1, d_b);
  add_kernel.setArg(2, d_c);

  // Enqueue the kernel with automatically determined work groups
  queue.enqueueNDRangeKernel(add_kernel, 0, N);

  // Retreive new values
  queue.enqueueReadBuffer(d_c, CL_TRUE, 0, size, &h_c.front());

  // Display results
  for (int i = 0; i < N; i++)
    cout << h_c[i] << ", ";

  return EXIT_SUCCESS;
}
