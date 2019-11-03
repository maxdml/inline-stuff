#include <thread>

#include "msr.hh"

#define N_FIXED_CTR 0
#define N_CUSTOM_CTR 2
#define MAX_PTS 50000

/*********************
 * Utility functions *
 *********************/
/**
 * Serialize instructions before RDTSC, then gather ellapsed cycles
 */
static inline uint64_t rdtsc_start() {
    unsigned cycles_low, cycles_high;
    asm volatile (
        "CPUID\n\t"
        "RDTSC\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low):: "%rax", "%rbx", "%rcx", "%rdx"
    );
    return (((uint64_t)cycles_high << 32) | cycles_low);
}

/**
 * Gather ellapsed cycles, protecting RDTSCP and mov insns from future insn with CPUID
 */
static inline uint64_t rdtsc_end() {
    unsigned cycles_low, cycles_high;
    asm volatile(
        "RDTSCP\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        "CPUID\n\t": "=r" (cycles_high), "=r" (cycles_low):: "%rax", "%rbx", "%rcx", "%rdx"
    );
    return (((uint64_t)cycles_high << 32) | cycles_low);
}

static uint64_t mat_rng[2] = {11ULL, 1181783497276652981ULL};
static inline uint64_t xorshift128plus(uint64_t s[2]) {
    uint64_t x, y;
    x = s[0], y = s[1];
    s[0] = y;
    x ^= x << 23;
    s[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
    y += s[1];
    return y;
}

/*********************
 * Threads arguments *
 *********************/
template<typename T, uint8_t width, uint64_t height>
class MultiArray {
    private:
        typedef T cols[height];
        cols * data;
    public:
        T& operator() (uint8_t x, uint64_t y) { return data[x][y]; }
        MultiArray() { data = new cols[width]; }
        ~MultiArray() { delete [] data; }
};

struct ThreadArgs {
    uint32_t iterations = 0;

    /* Profiling */
    MultiArray<uint64_t, N_CUSTOM_CTR + N_FIXED_CTR, MAX_PTS> values;
    uint64_t counts = 0;
    MsrHandle *cpu_msr = nullptr;
};

/*********************
 * Benchmark threads & MSR utilities *
 *********************/
class BenchmarkThread {
    public:
        std::thread *t;
        struct ThreadArgs args;

        ~BenchmarkThread() {
            delete t;
        }
};

static void read_values(MsrHandle *cpu_msr, uint64_t *store) {
    for (int i = 0; counter_tbl[i].name; ++i) {
        cpu_msr->read(counter_tbl[i].pmc, &store[i]);
        printf("value of counter %d is %lu\n", i, store[i]);
    }
}

/***********************
 * Benchmark functions *
 ***********************/
void run_test_benchmark(MsrHandle* cpu_msr);
void cache_work(struct ThreadArgs &args);
void file_work(struct ThreadArgs args);
