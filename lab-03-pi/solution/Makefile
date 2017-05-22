# Set env. variables
include ../../Makefile.default

INCLUDES=../../

# Target options: sw_emu|hw_emu|hw
XOCCFLAGS += -t sw_emu

.PHONY:
all: pi pi_initial.xclbin pi_vector_types.xclbin pi_async_copy.xclbin pi_vector_with_reduction.xclbin pi_async_with_reduction.xclbin

.PHONY:
check: all
	XCL_EMULATION_MODE=sw_emu ./pi

pi: pi.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -I$(INCLUDES) -I$(OPENCL_INC) -o $@ pi.cpp

pi_async_copy.xclbin: pi_async_copy.cl
	$(XOCC) --memory_port_data_width all:512 $(XOCCFLAGS) $< -o $@

pi_async_with_reduction.xclbin: pi_async_with_reduction.cl
	$(XOCC) --memory_port_data_width all:512 $(XOCCFLAGS) $< -o $@

%.xclbin: %.cl
	$(XOCC) $(XOCCFLAGS) $< -o $@

clean:
	rm -rf pi *.xclbin *.ll *.csv *.html *.log shared*
