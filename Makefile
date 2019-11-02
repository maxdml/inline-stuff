EXEC = main.x  cpuid.x msr.x

all: $(EXEC)

CXXFLAGS += -Wall -ggdb -O0
CXXFLAGS += -std=c++17
#CXXFLAGS += -lasan -fsanitize=address
LIB = -lpthread
#LIB += -fsanitize=address

COMMON_OBJS = main.o msr.o benchmarks.o
OBJS = $(COMMON_OBJS)

.PRECIOUS: $(OBJS)

%.x: %.o $(COMMON_OBJS)
	$(CXX) -o $@ $^ $(LIB)

cpuid.x: cpuid.c msr-old.c
	$(CXX) -o $@ $^

.PHONY: clean

clean:
	rm *.o *.x
