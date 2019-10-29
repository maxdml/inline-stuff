#include <vector>
#include <cstdint>
#include <iostream>
#include <thread>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "msr.hh"

#define MAX_PTS 50000
/**********************************
 * Pseudo-random number generator *
 **********************************/

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

uint32_t drand(void) {
	return (xorshift128plus(mat_rng)>>11);
}

static struct counter_table_t counter_tbl[] = {
    {"L2_HITS",         CUSTOM_CTR,     MEM_LOAD_RETIRED_L2_HIT_EVTNR,      MEM_LOAD_RETIRED_L2_HIT_UMASK,  IA32_PERFEVTSEL0_ADDR,  IA32_PMC0,                      0},
    {"L2_MISSES",       CUSTOM_CTR,     MEM_LOAD_RETIRED_L2_MISS_EVTNR,     MEM_LOAD_RETIRED_L2_MISS_UMASK, IA32_PERFEVTSEL1_ADDR,  IA32_PMC1,                      1},
    {"L3_HITS",         CUSTOM_CTR,     MEM_LOAD_RETIRED_L2_HIT_EVTNR,      MEM_LOAD_RETIRED_L2_HIT_UMASK,  IA32_PERFEVTSEL2_ADDR,  IA32_PMC2,                      2},
    {"L3_MISSES",       CUSTOM_CTR,     MEM_LOAD_RETIRED_L2_MISS_EVTNR,     MEM_LOAD_RETIRED_L2_MISS_UMASK, IA32_PERFEVTSEL3_ADDR,  IA32_PMC3,                      3},
    {"CPU_CYCLES",      FIXED_CTR,      0x0,                                0x0,                            IA32_CR_FIXED_CTR_CTRL, CPU_CLK_UNHALTED_THREAD_ADDR,   4},
    {"INSN_RETIRED",    FIXED_CTR,      0x0,                                0x0,                            IA32_CR_FIXED_CTR_CTRL, INST_RETIRED_ANY_ADDR,          5},
};


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

template<typename T, uint8_t width, uint64_t height>
class MultiArray
{
    private:
        typedef T cols[height];
        cols * data;
    public:
        T& operator() (uint8_t x, uint64_t y) { return data[x][y]; }
        MultiArray() { data = new cols[width]; }
        ~MultiArray() { delete [] data; }
};

struct ThreadArgs {
    uint32_t iterations;

    /* matrix & array benchmarks */
    uint32_t a[100][100]; // Around 4B*10k = 40KB

    /* File benchmarks */
    std::string f1;
    std::string f2;

    /* Profiling */
    MultiArray<uint64_t, PERF_MAX_CUSTOM_COUNTERS + PERF_MAX_FIXED_COUNTERS, MAX_PTS> values;
    uint64_t counts;
    MsrHandle *cpu_msr;
};

/* Benchmark threads */
class BenchmarkThread {
    public:
        std::thread *t;
        struct ThreadArgs args;
};

void read_values(MsrHandle *cpu_msr, uint64_t *store) {
    for (int i = 0; counter_tbl[i].name; ++i) {
        cpu_msr->read(counter_tbl[i].pmc, &store[i]);
    }
}

void cache_work(struct ThreadArgs args) {
    uint64_t store_start[PERF_MAX_CUSTOM_COUNTERS + PERF_MAX_FIXED_COUNTERS];
    uint64_t store_end[PERF_MAX_CUSTOM_COUNTERS + PERF_MAX_FIXED_COUNTERS];
    uint32_t i, j, k;
    for (k = 0; k < args.iterations; ++k) {
        read_values(args.cpu_msr, store_start);
        for (j = 0; j < 100; ++j) {
            for (i = 0; i < 100; ++i) {
                args.a[i][j] = 2 * args.a[i][j];
            }
        }
    /*
        for (i = 0; i < 5000; ++i) {
            for (j = 0; j < 100; ++j) {
                args.a[i][j] = 2 * args.a[i][j];
            }
        }
    */
        read_values(args.cpu_msr, store_end);
        for (int i = 0; counter_tbl[i].name; ++i) {
            args.values(i, args.counts) = store_end[i] - store_start[i];
        }
        args.counts++;
    }
}

void file_work(struct ThreadArgs args) {
    for (uint32_t i = 0; i < args.iterations; ++i) {
        FILE *file = NULL;
        if (i%2 == 0) {
            file = fopen(args.f1.c_str(), "rb");
        } else {
            file = fopen(args.f2.c_str(), "rb");
        }
        if (fseek(file, 0, SEEK_END) == -1) {
            std::cout << "fseek error: " << strerror(errno) << std::endl;
        }
        int size = ftell(file);
        if (size == -1) {
            std::cout << "Error telling file size: " << strerror(errno) << std::endl;
        }
        if (fseek(file, 0, SEEK_SET) == -1) {
            std::cout << "fseek error: " << strerror(errno) << std::endl;
        }
        char *body = static_cast<char *>(malloc(size+1));
        size_t char_read = fread(body, sizeof(char), size, file);
        if (char_read < (unsigned) size) {
            std::cout << "fread() read less bytes than file's size." << std::endl;
        }
        if (strstr(body, "somedummystring") != NULL) {
            std::cout << "Dummy string match. Stunning." << std::endl;
        }
        free(body);
        body = NULL;
        fclose(file);
    }
    pthread_exit(NULL);
}

void pin_thread(pthread_t thread, u_int16_t cpu) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);

    int rtn = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (rtn != 0) {
        fprintf(stderr, "could not pin thread: %s\n", strerror(errno));
    }
}

// Just less than L2:
// 1023K /media/memfs/wiki/fr/h/e/x/Utilisateur~Hexasoft_Statistiques_Geckonidé_41ae.html
// 1018K /media/memfs/wiki/fr/c/l/n/Utilisateur~Cln-id_monobook.js_9900.html

// Just above L2:
// 1.2M /media/memfs/wiki/fr/b/u/l/Wikipédia~Bulletin_des_administrateurs_Février_2007_8e0a.html
// 1.2M /media/memfs/wiki/fr/s/o/b/Utilisateur~Sobreira_Carnivora_a3cd.html

// less than 2 L3 ways:
// 2.2M /media/memfs/wiki/fr/i/t/a/Projet~Italie_Catégorisation_f5a3.html
// 2.1M /media/memfs/wiki/fr/z/x/8/Utilisateur~ZX81-bot_Journaux_2007042701_125f.html

struct CustomCoreEventDescription {
    int32_t event_number;
    int32_t umask_value;
};

void configure_counters(MsrHandle * cpu_msr[],
                        std::vector<uint8_t> &cpus) {
    uint32_t res;
    for (unsigned int i = 0; i < cpus.size(); ++i) {
        cpu_msr[i] = new MsrHandle(cpus[i]);
        assert(cpu_msr[i] != nullptr);

        /* Disable counters while programming */
        cpu_msr[i]->write(IA32_CR_PERF_GLOBAL_CTRL, 0);

        /* Reset all PMCs */
        for (int c = 0; c < PERF_MAX_CUSTOM_COUNTERS; ++c) {
            res = cpu_msr[i]->write(IA32_PMC0 + c, 0);
        }

        /* Disable fixed registers */
        FixedEventControlRegister ctrl_reg;
        ctrl_reg.value = 0xffffffffffffffff;
        cpu_msr[i]->read(IA32_CR_FIXED_CTR_CTRL, &ctrl_reg.value);
        if ((ctrl_reg.fields.os0 ||
             ctrl_reg.fields.usr0 ||
             ctrl_reg.fields.enable_pmi0 ||
             ctrl_reg.fields.os1 ||
             ctrl_reg.fields.usr1 ||
             ctrl_reg.fields.enable_pmi1 ||
             ctrl_reg.fields.os2 ||
             ctrl_reg.fields.usr2 ||
             ctrl_reg.fields.enable_pmi2)
            != 0)
            cpu_msr[i]->write(IA32_CR_FIXED_CTR_CTRL, 0);

        /* Configure fixed registers */
        res = cpu_msr[i]->read(IA32_CR_FIXED_CTR_CTRL, &ctrl_reg.value);
        assert(res >= 0);

        ctrl_reg.fields.os0 = 1;
        ctrl_reg.fields.usr0 = 1;
        ctrl_reg.fields.any_thread0 = 0;
        ctrl_reg.fields.enable_pmi0 = 0;

        ctrl_reg.fields.os1 = 1;
        ctrl_reg.fields.usr1 = 1;
        ctrl_reg.fields.any_thread1 = 0;
        ctrl_reg.fields.enable_pmi1 = 0;

        ctrl_reg.fields.os2 = 1;
        ctrl_reg.fields.usr2 = 1;
        ctrl_reg.fields.any_thread2 = 0;
        ctrl_reg.fields.enable_pmi2 = 0;

        cpu_msr[i]->write(IA32_CR_FIXED_CTR_CTRL, ctrl_reg.value);

        /* Enables 3 fixed counters (bits 32, 33 and 34) */
        uint64_t value = (1ULL << 32) + (1ULL << 33) + (1ULL << 34);

        /* Configure PMC. We expect counter_tbl to declare PMC first and then fixed counters */
        for (int c = 0; counter_tbl[c].name; ++c) {
            if (c > (PERF_MAX_FIXED_COUNTERS + PERF_MAX_CUSTOM_COUNTERS)) {
                break;
            }
            if (counter_tbl[c].ctr_type == CUSTOM_CTR) {
                EventSelectRegister evt_reg;
                res = cpu_msr[i]->read(counter_tbl[c].cfg_reg + c, &evt_reg.value);
                assert (res >= 0);

                evt_reg.value = 0;

                evt_reg.fields.usr = 1;
                evt_reg.fields.os = 1;
                evt_reg.fields.enable = 1;
                evt_reg.fields.event_select = counter_tbl[c].evtnr;
                evt_reg.fields.umask = counter_tbl[c].umask;
                evt_reg.fields.edge = 1;
                evt_reg.fields.any_thread = 0;
                evt_reg.fields.invert = 0;
                evt_reg.fields.cmask = 0;
                evt_reg.fields.in_tx = 0;
                evt_reg.fields.in_txcp = 0;
                evt_reg.fields.pin_control = 0;
                evt_reg.fields.apic_int = 0;

                res = cpu_msr[i]->write(counter_tbl[c].cfg_reg + c, evt_reg.value);
                assert (res >= 0);

                value += (1ULL << counter_tbl[c].id);
            }
        }

        res = cpu_msr[i]->write(IA32_CR_PERF_GLOBAL_CTRL, value);
        assert(res >= 0);
    }
}

//TODO
// - disable/reset everything once done
int main() {
    pin_thread(pthread_self(), 0);
    std::vector<uint8_t> cpus {6}; // 0-indexed
    MsrHandle * cpu_msr[cpus.size()];

    /* Configure counters for each core */
    configure_counters(cpu_msr, cpus);

    /* Setup file micro benchmarks */
    std::string f1 = "/media/memfs/wiki/fr/h/e/x/Utilisateur~Hexasoft_Statistiques_Geckonidé_41ae.html";
    std::string f2 = "/media/memfs/wiki/fr/c/l/n/Utilisateur~Cln-id_monobook.js_9900.html";
    //std::string f1 = "/media/memfs/wiki/fr/b/u/l/Wikipédia~Bulletin_des_administrateurs_Février_2007_8e0a.html";
    //std::string f2 = "/media/memfs/wiki/fr/s/o/b/Utilisateur~Sobreira_Carnivora_a3cd.html";
    //std::string f1 = "/media/memfs/wiki/fr/i/t/a/Projet~Italie_Catégorisation_f5a3.html";
    //std::string f2 = "/media/memfs/wiki/fr/z/x/8/Utilisateur~ZX81-bot_Journaux_2007042701_125f.html";

    /* Setup arrays & matrix micro benchmarks */
    struct ThreadArgs aargs;
    aargs.iterations = 1000;
	uint32_t i, j;
	for (i = 0; i < 100; ++i) {
	    for (j = 0; j < 100; ++j) {
            aargs.a[i][j] = xorshift128plus(mat_rng) >> 11;
            __asm__ volatile("clflush (%0)" : : "r" ((volatile void *)& aargs.a[i][j]) : "memory");
        }
    }

    uint32_t iterations = 10000;

    std::vector<std::unique_ptr<BenchmarkThread> > threads;
    /* Setup benchmark threads */
    for (unsigned int i = 0; i < cpus.size(); ++i) {
        std::unique_ptr<BenchmarkThread> t(new BenchmarkThread);
        t->args.iterations = iterations;
        t->args.cpu_msr = cpu_msr[i];
        threads.push_back(std::move(t));
    }

    /* Setup their worker and specific input */
    /*
    threads[0]->args.f1 = f1;
    threads[0]->args.f2 = f1;
    threads[0]->t = new std::thread(file_work, threads[0]->args);
    pin_thread(threads[0]->t->native_handle(), cpus[0]);
    */

    /*
    threads[1]->args.f1 = f1;
    threads[1]->args.f2 = f1;
    threads[1]->t = new std::thread(file_work, threads[1]->args);
    pin_thread(threads[1]->t->native_handle(), cpus[1]);
    */

    threads[0]->args = aargs;
    threads[1]->t = new std::thread(cache_work, threads[0]->args);
    /*
    threads[1]->args = aargs;
    threads[1]->t = new std::thread(cache_work, aargs);
    */

    FILE *f = fopen("mb.csv", "w");
    if (f) {
        fprintf(f, "TID");
        uint8_t i = 0;
        do { //FIXME: while this helps for printing, this will break if counter_tbl[i] is invalid
            fprintf(f, "\t%s", counter_tbl[i].name);
            i++;
        } while (counter_tbl[i].name);
        fprintf(f, "\n");
    } else {
        fprintf(stderr, "Could not open CSV file\n");
    }
    for (unsigned int i = 0; i < threads.size(); ++i) {
        threads[i]->t->join();
        for (uint64_t j = 0; j < threads[i]->args.counts; ++j) {
            fprintf(f, "%u", i);
            uint8_t c = 0;
            do {
                fprintf(f, "\t%lu", threads[i]->args.values(c, j));
                c++;
            } while (counter_tbl[c].name);
            fprintf(f, "\n");
        }
    }
    fclose(f);

    return 0;
}
