

# Set environmental variables 

Add the following lines to your `~/.bashrc` file and restart your shell
to set several environmental variables required for compilation.

```
export XILINX_OPENCL=/opt/Xilinx/SDAccel/2015.4
# LD_LIBRARY_PATH setting, replace sdaccel installation with the location of 
# SDAccel in the local system
export LD_LIBRARY_PATH=$XILINX_OPENCL/runtime/lib/x86_64/:$LD_LIBRARY_PATH

# LD_LIBRARY_PATH setting, replace sdaccel installation with the location of 
# SDAccel in the local system
export LD_LIBRARY_PATH=$XILINX_OPENCL/lib/lnx64.o/:$LD_LIBRARY_PATH

# Set the target platform for the application 
# (this MUST match your hardware + software version!)
export XCL_PLATFORM=xilinx_adm-pcie-7v3_1ddr_2_1
```

# cl.hpp

Our demos use the C++ OpenCL API; however, the 2015.4 release of Xilinx SDAccel 
ships with an outdated version of `cl.hpp` which causes missing symbol errors
when the exercises are compiled. Simply run `sudo ./fix_cl_hpp.sh` in the root
directory of this folder

has a mis-match between the OpenCL version and `cl.hpp` file which causes 
missing symbol errors when your code is complied. Simply run

    sudo fix_cl_hpp.sh
    
to automatically back up the default version of `cl.hpp`, download a standard
version from Khronos.org, and install it to the appropriate directory.
