#include <stdio.h>
#include <iostream>
#include <stdint.h>

void msr(uint64_t addr, uint32_t* eax, uint32_t* edx) 
{
    //asm volatile
    //  ("rdmsr" 
    //   : "=a" (*eax), "=d" (*edx) 
    //   : "c" (addr)
    //  );
}


