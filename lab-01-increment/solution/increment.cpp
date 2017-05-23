/*
  ArrayFire OpenCL Training
  Lab: Variable increment

  Description:
  This application will increment a variable on the FPGA using OpenCL
*/

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using std::ifstream;
using std::ios;
using std::istreambuf_iterator;
using std::string;
using std::vector;

// Reads a file from disk given a filename
vector<char> readBinary(const string &fileName) {
  ifstream file(fileName, ios::binary);
  if (!file) {
    fprintf(stderr, "%s not found\n", fileName.c_str());
    exit(EXIT_FAILURE);
  }
  vector<char> buffer((istreambuf_iterator<char>(file)),
                      istreambuf_iterator<char>());
  return buffer;
}

using namespace std;

int main(void) {
  // Get all the available OpenCL platforms on the system
  vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);

  // (Optional) Remove other platforms on the machine
  for (size_t i = 0; i < platforms.size(); i++) {
    if (platforms[i].getInfo<CL_PLATFORM_NAME>().find("Xilinx") ==
        string::npos) {
      platforms.erase(begin(platforms) + i);
      i--;
    }
  }

  // Get all the devices on the first platform
  vector<cl::Device> devices;
  platforms[0].getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);

  // Create an OpenCL context based on the first device
  cl::Context context(devices[0]);

  // (Optional) Display device information
  string name = devices[0].getInfo<CL_DEVICE_NAME>();
  printf("Device Names: %s\n", name.c_str());

  // Create a command queue
  cl::CommandQueue queue(context, devices[0]);

  // Read in the pre-compiled kernel
  vector<char> contents = readBinary("./increment.xclbin");

  // Create an OpenCL binary from which we will create a program
  cl::Program::Binaries binaries;
  binaries.push_back(std::make_pair(contents.data(), contents.size()));

  // Create an OpenCL program from which we can extract symbols
  cl::Program increment_program(context, devices, binaries);

  // After the program is created, we can create kernels
  cl::Kernel increment_kernel(increment_program, "increment");

  // Create an input buffer with one element
  cl::Buffer val(context, CL_MEM_READ_WRITE, sizeof(int));

  // Copy the value to the device
  int hostVal = 40;
  printf("Before: %d\n", hostVal);
  queue.enqueueWriteBuffer(val, CL_FALSE, 0, sizeof(int), &hostVal);

  // Set the kernel arguments and launch the kernel.
  increment_kernel.setArg(0, val);
  queue.enqueueNDRangeKernel(increment_kernel, 0, 1);

  // Read the result from the device and print it.
  queue.enqueueReadBuffer(val, CL_TRUE, 0, sizeof(int), &hostVal);
  printf("After: %d\n", hostVal);

  return EXIT_SUCCESS;
}
