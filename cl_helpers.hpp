#ifndef CL_HELPERS_HPP
#define CL_HELPERS_HPP

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include <fstream>
#include <iostream>
#include <string>

// Returns the contents of the file <fileName> as a string
std::string readFile(const std::string &fileName) {
  std::string output;
  std::ifstream file(fileName.c_str(), std::ios::in);
  getline(file, output, std::string::traits_type::to_char_type(
                            std::string::traits_type::eof()));
  return output;
}

// A callback function which displays the log of the program build
void programCallback(cl_program program, void *user_data) {
  clRetainProgram(program);
  cl::Program p(program);
  cl::Device dev_id = p.getInfo<CL_PROGRAM_DEVICES>()[0];
  std::string device_name = dev_id.getInfo<CL_DEVICE_NAME>();
  std::string log = p.getBuildInfo<CL_PROGRAM_BUILD_LOG>(dev_id);
  std::cout << device_name << " Log: " << log << std::endl;
}

std::vector<char> readBinary(const std::string &fileName) {
  std::ifstream file(fileName.c_str(), std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size);
  file.read(buffer.data(), size);
  return buffer;
}

#endif
