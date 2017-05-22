# Set env. variables
include ../Makefile.default

INCLUDES=../

# Target options: sw_emu|hw_emu|hw
XOCCFLAGS+=-t sw_emu

.PHONY: all
all: vector_addition vector_addition_xclbin

.PHONY:
vector_addition_xclbin: vector_addition.xclbin

.PHONY: exe
all: vector_addition vector_addition_xclbin

%: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -I$(INCLUDES) -I$(OPENCL_INC) -L$(OPENCL_LIB) -lOpenCL -o $@ $<

%.xclbin: %.cl
	$(XOCC) $(XOCCFLAGS) $(CLFLAGS) $< -o $@

clean:
	rm -rf vector_addition vector_addition.xclbin vector_addition.ll *.csv *.html *.log
