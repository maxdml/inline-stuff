#include <iostream>
#include <stdio.h>
#include <stdint.h>

using namespace std ;

static inline uint64_t rdtsc() {
 uint64_t a = 0, d = 0;
 asm volatile("mfence");
 asm volatile("rdtscp" : "=a"(a), "=d"(d) :: "rcx");
 a = (d << 32) | a;
 asm volatile("mfence");
 return a;
}


int main() {

    int Timeme = 100 ;

    uint64_t start = 0 , end  = 0 ;

    __asm__ volatile("clflush (%0)" : : "r" ((volatile void *)&Timeme) : "memory"); // Flush the address from the cache

    start = rdtsc();
    volatile int temp = Timeme; // Read the data from the RAM ( Also brings it to the cache)
    end = rdtsc();

    cout << "Time taken without Caching "<< end-start <<endl;

    start = rdtsc();
    volatile int temp_cache = Timeme; // Timeme now already resides in cache
    end = rdtsc();

    cout << "Time taken with Caching "<< end-start <<endl;


    return 0 ;

}
