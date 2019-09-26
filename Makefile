CXX=g++
CPPFLAGS=-ggdb

TARGETS = cpuid

cpuid: cpuid.c
	$(CXX) -o $@ $^ $(CPPFLAGS)

.PHONY: clean

clean:
	rm -f *.o $(TARGETS) core
