SRCS = src/bignum-dtoa.cc src/bignum.cc src/cached-powers.cc src/diy-fp.cc src/double-conversion.cc src/fast-dtoa.cc src/fixed-dtoa.cc src/strtod.cc
DCLIB = libdoubleconversion.a
OBJS = $(SRCS:.cc=.o)

ifeq ($(RANLIB),)
	RANLIB=ranlib
endif

CXXFLAGS = -Wall -g -O2
LOADLIBES = -L./src

$(DCLIB): $(OBJS)
	$(AR) $(ARFLAGS) $(DCLIB) $(OBJS)
	$(RANLIB) $(DCLIB)

.cc.o:
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(DCLIB)
