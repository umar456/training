#!/bin/bash

XILINX_SDACCEL=/opt/Xilinx/SDx/2016.4
CL_INCLUDE=${XILINX_SDACCEL}/runtime/include/1_2/CL/

wget https://www.khronos.org/registry/cl/api/2.1/cl.hpp
sudo mv ${CL_INCLUDE}/cl.hpp ${CL_INCLUDE}/cl.hpp.back
sudo mv cl.hpp ${CL_INCLUDE}/cl.hpp
