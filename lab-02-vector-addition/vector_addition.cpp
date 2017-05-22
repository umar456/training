/*
  ArrayFire Training Day 1
  Lab: Vector Addition

  This program will add two vectors and store the result in a third vector
  using the FPGA
*/

#define __CL_ENABLE_EXCEPTIONS

#include <iostream>
#include <vector>
#include <CL/cl.hpp>
#include "cl_helpers.hpp"

using namespace std;

int main(void) {
    cl::Context context(CL_DEVICE_TYPE_ALL);
    vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
    for(cl::Device device : devices) {
        cout << "Device Name: " << device.getInfo<CL_DEVICE_NAME>() << endl;;
    }

    cl::CommandQueue queue(context, devices[0]);

    vector<char> contents = readBinary("vector_addition.xclbin");
    cl::Program::Binaries binaries;
    binaries.push_back(std::make_pair(contents.data(), contents.size()));

    cl::Program program(context, devices, binaries);

    vector<cl::Kernel> kernels;
    program.createKernels(&kernels);

    // Setup data and create buffers

    // Setup and launch kernel

    // Enqueue the kernel to operate on N elements with automatically determined
    // local sizes

    // Retrieve new values

    // Display results

    return EXIT_SUCCESS;
}
