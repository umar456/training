/*
  ArrayFire OpenCL Training
  Lab 01: Variable increment

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

int main(void)
{
  // Get all the available OpenCL platforms on the system

  // Get all of the devices on the platform

  // Create an OpenCL context with the first device

  // Create a command queue

  // Read in the pre-compiled kernel

  // Create an OpenCL binary from which we will create a program

  // Create a OpenCL program from which we can extract kernels

  // After the program is created, we can create the kernels

  // Create an input buffer with one element

  // Copy the value to the device

  // Set kernel arguments and launch the kernel

  // Read the result back from the device and print it.

  return EXIT_SUCCESS;
}
