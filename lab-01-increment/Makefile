# Set env. variables
include ../Makefile.default

# Target options: sw_emu|hw_emu|hw
XOCCFLAGS+=-t sw_emu

.PHONY: all
all: increment increment_xclbin

.PHONY:
increment_xclbin: increment.xclbin

.PHONY: exe
all: increment increment_xclbin

%: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -I$(OPENCL_INC) -L$(OPENCL_LIB) -lOpenCL -o $@ $<

%.xclbin: %.cl
	$(XOCC) $(XOCCFLAGS) $< -o $@

clean:
	rm -rf increment increment.xclbin increment.ll *.csv *.html *.log
