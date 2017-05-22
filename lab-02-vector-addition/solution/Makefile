# Set env. variables
include ../../Makefile.default

INCLUDES=../../

# Target options: sw_emu|hw_emu|hw
XOCCFLAGS+=-t sw_emu

.PHONY: all
all: vector_addition vector_addition_xclbin

.PHONY:
vector_addition_xclbin: vector_addition.xclbin

.PHONY: exe
all: vector_addition vector_addition_xclbin

.PHONY:
check: all
	XCL_EMULATION_MODE=sw_emu ./vector_addition

%: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -I$(OPENCL_INC) -I$(INCLUDES) -o $@ $<

%.xclbin: %.cl
	$(XOCC) $(XOCCFLAGS) $(CLFLAGS) $< -o $@

clean:
	rm -rf vector_addition vector_addition.xclbin vector_addition.ll *.csv *.html *.log
