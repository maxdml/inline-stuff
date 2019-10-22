#include <stdio.h>
#include <stdint.h>
#include <iostream>

class msr 
{  
    int fd;
    uint8_t cpu;

public:
    msr(uint8_t cpu_id);
    ~msr();

    bool msr_read(uint64_t reg, uint64_t *data);
    bool msr_write(uint64_t reg, uint64_t *data);
};
