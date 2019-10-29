#include <stdio.h>
#include <stdint.h>

/**  Global counters */
typedef enum
{
    PMC0,
    PMC1,
    PMC2,    
    PMC3,
    PMC4,
    PMC5,
    PMC6,
    PMC7,
    FIXED0,
    FIXED1,
    FIXED2

}global_counters;

class msr 
{  
    int fd;
    uint8_t cpu;

public:
    msr(uint8_t cpu_id);
    ~msr();

    bool msr_read(uint64_t reg, uint64_t *data);
    bool msr_write(uint64_t reg, uint64_t *data);
    void enable_l3_cache_miss();
    void enable_l3_cache_reference();
    void enable_global_counters();
    void disable_l3_cache_miss();
    void disable_l3_cache_reference();
    void disable_global_counters();
    uint64_t get_l3_cache_misses();
    uint64_t get_l3_cache_references();
};
