EXEC = main.x msrtest.x

all: $(EXEC)

CXXFLAGS += -Wall -ggdb -O0
CXXFLAGS += -std=c++17
CXXFLAGS += -lasan -fsanitize=address
LIB = -lpthread -fsanitize=address
LIB += -fsanitize=address

COMMON_OBJS = msr.o
EXE_OBJS = $(EXEC:.x=.o)
OBJS = $(COMMON_OBJS) $(EXEC_OBJS)

.PRECIOUS: $(OBJS)

%.x: %.o $(COMMON_OBJS)
	$(CXX) -o $@ $^ $(LIB)

cpuid: cpuid.c msr-old.c
	$(CXX) -o $@.x $^

.PHONY: clean

clean:
	rm *.o *.x
