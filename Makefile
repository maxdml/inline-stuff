EXEC = main.x  cpuid.x msr.x

all: $(EXEC)

CXXFLAGS += -Wall -O0 -ggdb
CXXFLAGS += -std=c++17
#CXXFLAGS += -lasan -fsanitize=address
LIB = -lpthread -lboost_program_options
#LIB += -fsanitize=address

OBJS = main.o msr.o benchmarks.o
DEPS = $(OBJS:.o=.d)

.PRECIOUS: $(OBJS)

# Linking
%.x: $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LIB)

-include $(DEP)
# Objects and dependencies
%.o: %.cc
	$(CXX) -c $(CXXFLAGS) $^ -o $@ $(LIB)
	$(CXX) -MM $(CXXFLAGS) $^ > $(@:.o=.d)

cpuid.x: cpuid.c msr-old.c
	$(CXX) -o $@ $^

.PHONY: clean

clean:
	rm *.o *.x *.d
