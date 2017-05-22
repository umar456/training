# Set env. variables
include ../Makefile.default

INCLUDES=../

# Target options: sw_emu|hw_emu|hw
XOCCFLAGS+=-t sw_emu

.PHONY:
all: pi pi_initial_xclbin

.PHONY:
pi_initial_xclbin: pi_initial.xclbin

pi: pi.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -I$(INCLUDES) -I$(OPENCL_INC) -o $@ pi.cpp

%.xclbin: %.cl
	$(XOCC) $(XOCCFLAGS) $< -o $@

clean:
	rm -rf pi *.xclbin *.ll *.csv *.html *.log
