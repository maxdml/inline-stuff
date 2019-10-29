#ifndef TYPES_H_
#define TYPES_H_

// Skylake PMC: p 3411
// check Section 18.6.3 through Section 18.3.4.5,
// then the differences Table 18-22
// 18.3.8.1.1 PEBS Data Format

#define PERF_MAX_CUSTOM_COUNTERS 4
#define PERF_MAX_FIXED_COUNTERS 3

#define IA32_PERF_CAPABILITIES (0x345)

/* See Section 17.17, “Time-Stamp Counter.” */
#define IA32_TIME_STAMP_COUNTER         (0x10)

/*
"Intel 64 and IA-32 Architectures Software Developers Manual Volume 3B:
System Programming Guide, Part 2", Appendix A "PERFORMANCE-MONITORING EVENTS"
and Volume 4. (p 4693 specifics to SkyLake are listed)
*/

/*
The IA32_FIXED_CTR_CTRL MSR include multiple sets of 4-bit field,
each 4 bit field controls the operation of a fixed-function performance counter.
*/
#define IA32_CR_FIXED_CTR_CTRL          (0x38D) //page 3361, Vol. 3B 18-7

/*
IA32_PERF_GLOBAL_CTRL MSR provides single-bit controls to enable counting of each performance counter.
*/
#define IA32_CR_PERF_GLOBAL_CTRL        (0x38F) // page 3362

/*
This event counts the number of instructions that retire
execution. For instructions that consist of multiple uops,
this event counts the retirement of the last uop of the
instruction. The counter continues counting during
hardware interrupts, traps, and in-side interrupt handlers.
*/
#define INST_RETIRED_ANY_ADDR           (0x309)

/*
The CPU_CLK_UNHALTED.THREAD event counts the number of core
cycles while the logical processor is not in a halt state.
If there is only one logical processor in a processor core,
CPU_CLK_UNHALTED.CORE counts the unhalted cycles of
the processor core.
The core frequency may change from time to time due to
transitions associated with Enhanced Intel SpeedStep
Technology or TM2. For this reason this event may have a
changing ratio with regards to time
*/
#define CPU_CLK_UNHALTED_THREAD_ADDR    (0x30A)

/*
This event counts the number of reference cycles at the
TSC rate when the core is not in a halt state and not in a TM
stop-clock state. The core enters the halt state when it is
running the HLT instruction or the MWAIT instruction. This
event is not affected by core frequency changes (e.g., P
states) but counts at the same frequency as the time stamp
counter. This event can approximate elapsed time while the
core was not in a halt state and not in a TM stopclock state.
*/
#define CPU_CLK_UNHALTED_REF_ADDR       (0x30B)

/*
    According to
    "Intel 64 and IA-32 Architectures Software Developers Manual Volume 3B:
    System Programming Guide, Part 2", Figure 30-7. Layout of
    IA32_FIXED_CTR_CTRL MSR Supporting Architectural Performance Monitoring Version 3
*/
struct FixedEventControlRegister {
    union {
        struct {
            // CTR0
            uint64_t os0 : 1;
            uint64_t usr0 : 1;
            uint64_t any_thread0 : 1;
            uint64_t enable_pmi0 : 1;
            // CTR1
            uint64_t os1 : 1;
            uint64_t usr1 : 1;
            uint64_t any_thread1 : 1;
            uint64_t enable_pmi1 : 1;
            // CTR2
            uint64_t os2 : 1;
            uint64_t usr2 : 1;
            uint64_t any_thread2 : 1;
            uint64_t enable_pmi2 : 1;

            uint64_t reserved1 : 52;
        } fields;
        uint64_t value;
    };
};

/* PCM 1 to 8 addresses */
#define IA32_PMC0 (0xC1)
#define IA32_PMC1 (0xC2)
#define IA32_PMC2 (0xC3)
#define IA32_PMC3 (0xC4)
#define IA32_PMC4 (0xC5)
#define IA32_PMC5 (0xC6)
#define IA32_PMC6 (0xC7)
#define IA32_PMC7 (0xC8)

// "Only IA32_PMC0 through IA32_PMC3 support PEBS." p 3403

/* Aliases to PCM for full width writable */
#define IA32_A_PMC0 (0x4C1);
#define IA32_A_PMC1 (0x4C2);
#define IA32_A_PMC2 (0x4C3);
#define IA32_A_PMC3 (0x4C4);
#define IA32_A_PMC4 (0x4C5);
#define IA32_A_PMC5 (0x4C6);
#define IA32_A_PMC6 (0x4C7);
#define IA32_A_PMC7 (0x4C8);

/* Event selector registers addresses */
#define IA32_PERFEVTSEL0_ADDR (0x186)
#define IA32_PERFEVTSEL1_ADDR (0x187)
#define IA32_PERFEVTSEL2_ADDR (0x188)
#define IA32_PERFEVTSEL3_ADDR (0x189)
#define IA32_PERFEVTSEL4_ADDR (0x18A)
#define IA32_PERFEVTSEL5_ADDR (0x18B)
#define IA32_PERFEVTSEL6_ADDR (0x18C)
#define IA32_PERFEVTSEL7_ADDR (0x18D)

/* Architectural performance events (vol 3B. 19.1) */
#define ARCH_LLC_REFERENCE_EVTNR        (0x2E)
#define ARCH_LLC_REFERENCE_UMASK        (0x4F)

#define ARCH_LLC_MISS_EVTNR     (0x2E)
#define ARCH_LLC_MISS_UMASK     (0x41)

/* Skylake events (Vol 3B. 19.2) */
#define L2_RQSTS_REFERENCES_EVTNR (0x24)
#define L2_RQSTS_REFERENCES_UMASK  (0xFF)

#define MEM_LOAD_RETIRED_L3_MISS_EVTNR (0xD1)
#define MEM_LOAD_RETIRED_L3_MISS_UMASK (0x20)

#define MEM_LOAD_RETIRED_L3_HIT_EVTNR (0xD1)
#define MEM_LOAD_RETIRED_L3_HIT_UMASK (0x04)

#define MEM_LOAD_RETIRED_L2_MISS_EVTNR (0xD1)
#define MEM_LOAD_RETIRED_L2_MISS_UMASK (0x10)

#define MEM_LOAD_RETIRED_L2_HIT_EVTNR (0xD1)
#define MEM_LOAD_RETIRED_L2_HIT_UMASK (0x02)

/*
    According to
    "Intel 64 and IA-32 Architectures Software Developers Manual Volume 3B:
    System Programming Guide, Part 2", Figure 30-6. Layout of IA32_PERFEVTSELx
    MSRs Supporting Architectural Performance Monitoring Version 3
*/
struct EventSelectRegister {
    union {
        struct {
            uint64_t event_select : 8;
            uint64_t umask : 8;
            uint64_t usr : 1;
            uint64_t os : 1;
            uint64_t edge : 1;
            uint64_t pin_control : 1;
            uint64_t apic_int : 1;
            uint64_t any_thread : 1;
            uint64_t enable : 1;
            uint64_t invert : 1;
            uint64_t cmask : 8;
            uint64_t in_tx : 1;
            uint64_t in_txcp : 1;
            uint64_t reservedX : 30;
        } fields;
        uint64_t value;
    };

    EventSelectRegister() : value(0) {}
};

#define FIXED_CTR 0x0
#define CUSTOM_CTR 0x1
struct counter_table_t {
    const char *name;
    uint64_t ctr_type;
    uint64_t evtnr;
    uint64_t umask;
    uint64_t cfg_reg;
    uint64_t pmc;
    uint64_t id;
};

#endif // TYPES_H_