#ifndef TYPES_H_
#define TYPES_H_

// Skylake PMC: p 3411
// check Section 18.6.3 through Section 18.3.4.5,
// then the differences Table 18-22
// 18.3.8.1.1 PEBS Data Format

#define PERF_MAX_CUSTOM_COUNTERS 8
#define PERF_MAX_FIXED_COUNTERS 3

/*================================================================================
  *                 REGISTER ADDRESSES
  *===============================================================================*/

/* See Section 17.17, “Time-Stamp Counter.” */
#define IA32_TIME_STAMP_COUNTER         (0x10)

/*
Intel 64 and IA-32 Architectures Software Developers Manual Volume 3B:
System Programming Guide, Part 2", Appendix A "PERFORMANCE-MONITORING EVENTS
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

/** Mainly used to check if full width write capability is enabled or not */
#define IA32_PERF_CAPABILITIES (0x345)

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

/** Bit offsets of counters in PERF_GLOBAL_CTRL register */
#define PERF_GLOBAL_CTRL_PMC0           0
#define PERF_GLOBAL_CTRL_PMC1           1
#define PERF_GLOBAL_CTRL_PMC2           2
#define PERF_GLOBAL_CTRL_PMC3           3
#define PERF_GLOBAL_CTRL_PMC4           4
#define PERF_GLOBAL_CTRL_PMC5           5
#define PERF_GLOBAL_CTRL_PMC6           6
#define PERF_GLOBAL_CTRL_PMC7           7
#define PERF_GLOBAL_CTRL_FIXED_CTR0     32
#define PERF_GLOBAL_CTRL_FIXED_CTR1     33
#define PERF_GLOBAL_CTRL_FIXED_CTR2     34

/*================================================================================
  *                  CPU SPECIFIC EVENTS
  *===============================================================================*/

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

/*================================================================================
  *                  ARCHITECTURAL EVENTS
  *===============================================================================*/
/* Architectural performance events (vol 3B. 19.1) */
#define ARCH_LLC_REFERENCE_EVTNR        (0x2E)
#define ARCH_LLC_REFERENCE_UMASK        (0x4F)

#define ARCH_LLC_MISS_EVTNR     (0x2E)
#define ARCH_LLC_MISS_UMASK     (0x41)

/*================================================================================
 *                  SKYLAKE EVENTS
 *===============================================================================*/
/* Skylake events (Vol 3B. 19.2) */

#define L2_RQSTS_REFERENCES_EVTNR (0x24)
#define L2_RQSTS_REFERENCES_UMASK  (0xFF)

#define L2_RQSTS_MISSES_EVTNR (0x24)
#define L2_RQSTS_MISSES_UMASK  (0x3F)

#define MEM_LOAD_RETIRED_L3_MISS_EVTNR (0xD1)
#define MEM_LOAD_RETIRED_L3_MISS_UMASK (0x20)

#define MEM_LOAD_RETIRED_L3_HIT_EVTNR (0xD1)
#define MEM_LOAD_RETIRED_L3_HIT_UMASK (0x04)

#define MEM_LOAD_RETIRED_L2_MISS_EVTNR (0xD1)
#define MEM_LOAD_RETIRED_L2_MISS_UMASK (0x10)

#define MEM_LOAD_RETIRED_L2_HIT_EVTNR (0xD1)
#define MEM_LOAD_RETIRED_L2_HIT_UMASK (0x02)

#define MEM_LOAD_RETIRED_L1_HIT_EVTNR (0xD1)
#define MEM_LOAD_RETIRED_L1_HIT_UMASK (0x01)

#define MEM_LOAD_RETIRED_L1_MISS_EVTNR (0xD1)
#define MEM_LOAD_RETIRED_L1_MISS_UMASK (0x08)

/*================================================================================
 *                  HASWELL EVENTS
 *===============================================================================*/

#define HW_L2_RQSTS_REFERENCES_EVTNR (0x24)
#define HW_L2_RQSTS_REFERENCES_UMASK  (0xFF)

#define HW_L2_RQSTS_MISSES_EVTNR (0x24)
#define HW_L2_RQSTS_MISSES_UMASK  (0x3F)

#define HW_L2_RQSTS_RFO_HITS_EVTNR                  (0x24)  /** number of store RFO requests that hit the L2 cache.*/
#define HW_L2_RQSTS_RFO_HITS_UMASK                  (0x42)
#define HW_L2_RQSTS_RFO_MISS_EVTNR                  (0x24)  /** number of store RFO requests that miss the L2 cache.*/
#define HW_L2_RQSTS_RFO_MISS_UMASK                  (0x22)
#define HW_L2_RQSTS_ALL_RFO_EVTNR                   (0x24)  /** all L2 store RFO requests */
#define HW_L2_RQSTS_ALL_RFO_UMASK                   (0xE2)
#define HW_L2_RQSTS_DEMAND_DATA_RD_MISS_EVTNR       (0x24)  /** Demand data read requests that missed L2, no rejects*/
#define HW_L2_RQSTS_DEMAND_DATA_RD_MISS_UMASK       (0x21)
#define HW_L2_RQSTS_DEMAND_DATA_RD_HIT_EVTNR        (0x24)  /** Demand data read requests that hit L2 cache. */
#define HW_L2_RQSTS_DEMAND_DATA_RD_HIT_UMASK        (0x41)
#define HW_L2_RQSTS_ALL_DEMAND_DATA_RD_EVTNR        (0x24)  /** any demand and L1 HW prefetch data load requests to L2*/
#define HW_L2_RQSTS_ALL_DEMAND_DATA_RD_UMASK        (0xE1)
#define HW_L2_RQSTS_L2_PF_HIT_EVTNR                 (0x24)  /** all L2 HW prefetcher requests that hit L2. */
#define HW_L2_RQSTS_L2_PF_HIT_UMASK                 (0x50)
#define HW_L2_RQSTS_L2_PF_MISS_EVTNR                (0x24)  /** all L2 HW prefetcher requests that missed L2.*/
#define HW_L2_RQSTS_L2_PF_MISS_UMASK                (0x30)
#define HW_L2_RQSTS_ALL_PF_EVTNR                    (0x24)  /** all L2 HW prefetcher requests */
#define HW_L2_RQSTS_ALL_PF_UMASK                    (0xF8)

#define HW_OFFCORE_REQUESTS_DEMAND_RFO_EVTNR        (0xB0)  // NOTE: Use only when HTT is off
#define HW_OFFCORE_REQUESTS_DEMAND_RFO_UMASK        (0x04)  
#define HW_OFFCORE_REQUESTS_DEMAND_DATA_RD_EVTNR    (0xB0)  // NOTE: Use only when HTT is off
#define HW_OFFCORE_REQUESTS_DEMAND_DATA_RD_UMASK    (0x01)

#define HW_TX_MEM_ABORT_CONFLICT_EVTNR              (0x54)
#define HW_TX_MEM_ABORT_CONFLICT_UMASK              (0x01)

#define HW_L2_TRANS_DEMAND_DATA_RD_EVTNR            (0xF0)  /** Demand data read requests that access L2 cache. */
#define HW_L2_TRANS_DEMAND_DATA_RD_UMASK            (0x01)  
#define HW_L2_TRANS_RFO_EVTNR                       (0xF0)  /** RFO requests that access L2 cache */
#define HW_L2_TRANS_RFO_UMASK                       (0x02)  
#define HW_L2_TRANS_L2_FILL_EVTNR                   (0xF0)  /** L2 fill requests that access L2 cache */   
#define HW_L2_TRANS_L2_FILL_UMASK                   (0x20)   

#define HW_L2_LINES_IN_I_EVTNR                      (0xF1)  /** L2 cache lines in I state filling L2. */
#define HW_L2_LINES_IN_I_UMASK                      (0x01)
#define HW_L2_LINES_IN_S_EVTNR                      (0xF1)  /** L2 cache lines in S state filling L2. */
#define HW_L2_LINES_IN_S_UMASK                      (0x02)
#define HW_L2_LINES_IN_E_EVTNR                      (0xF1)  /** L2 cache lines in E state filling L2. */
#define HW_L2_LINES_IN_E_UMASK                      (0x04)


/*===========================================================================================
  *             CONFIGURATION REGSITERS' STRUCTURES
  *==========================================================================================*/
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


/**
  * Cache information registers' structure
  */
typedef struct
{
    union
    {
        struct
        {
            uint32_t cache_type : 4;
            uint32_t reserved1 : 1;
            uint32_t level : 3;
            uint32_t self_init_level : 1;
            uint32_t fully_associative : 1;
            uint32_t reserved2 : 4;
            uint32_t logical_procs_sharing : 12;
            uint32_t physical_cores_in_package : 6;
        }eax_bit_values;
        uint32_t eax_value;
    }eax;
    union
    {
        struct
        {
            uint32_t cache_line_size : 12;
            uint32_t physical_line_partitions : 10;
            uint32_t ways : 10;
        }ebx_bit_values;
        uint32_t ebx_value;
    }ebx;
    union
    {
        struct
        {
            uint32_t num_sets : 32;
        }ecx_bit_values;
        uint32_t ecx_value;

    }ecx;
    union
    {
        struct
        {
            uint32_t unused : 1;
            uint32_t inclusive : 1;
            uint32_t direct_mapped : 1;
            uint32_t reserved : 29;
        }edx_bit_values;
        uint32_t edx_value;
    }edx;
}cache_params;

#define FIXED_CTR 0x0
#define CUSTOM_CTR 0x1
struct counter_table_t {
    const char *name;
    uint64_t ctr_type;
    uint64_t evtnr;
    uint64_t umask;
    uint64_t cfg_reg;
    uint64_t pmc;
    uint8_t id;
};

#if 1
static struct counter_table_t counter_tbl[] = {
    {"L2_REFS",         CUSTOM_CTR,     HW_L2_RQSTS_REFERENCES_EVTNR,               HW_L2_RQSTS_REFERENCES_UMASK,               IA32_PERFEVTSEL0_ADDR,  IA32_PMC0,                      0},
    {"L2_MISSES",       CUSTOM_CTR,     HW_L2_RQSTS_MISSES_EVTNR,                   HW_L2_RQSTS_MISSES_UMASK,                   IA32_PERFEVTSEL1_ADDR,  IA32_PMC1,                      1},
    {"L2_DD_HITS",      CUSTOM_CTR,     HW_L2_RQSTS_DEMAND_DATA_RD_HIT_EVTNR,       HW_L2_RQSTS_DEMAND_DATA_RD_HIT_UMASK,       IA32_PERFEVTSEL2_ADDR,  IA32_PMC2,                      2},
    {"L2_DD_MISSES",    CUSTOM_CTR,     HW_L2_RQSTS_DEMAND_DATA_RD_MISS_EVTNR,      HW_L2_RQSTS_DEMAND_DATA_RD_MISS_UMASK,      IA32_PERFEVTSEL3_ADDR,  IA32_PMC3,                      3},
    {"L2_RFO_HITS",     CUSTOM_CTR,     HW_L2_RQSTS_RFO_HITS_EVTNR,                 HW_L2_RQSTS_RFO_HITS_UMASK,                 IA32_PERFEVTSEL4_ADDR,  IA32_PMC4,                      4},
    {"L2_RFO_MISSES",   CUSTOM_CTR,     HW_L2_RQSTS_RFO_MISS_EVTNR,                 HW_L2_RQSTS_RFO_MISS_UMASK,                 IA32_PERFEVTSEL5_ADDR,  IA32_PMC5,                      5},
    //{"L2_PF_HITS",      CUSTOM_CTR,     HW_L2_RQSTS_L2_PF_HIT_EVTNR,                HW_L2_RQSTS_L2_PF_HIT_UMASK,                IA32_PERFEVTSEL4_ADDR,  IA32_PMC4,                      4},
    //{"L2_PF_MISSES",    CUSTOM_CTR,     HW_L2_RQSTS_L2_PF_MISS_EVTNR,               HW_L2_RQSTS_L2_PF_MISS_UMASK,               IA32_PERFEVTSEL5_ADDR,  IA32_PMC5,                      5},
    {"L3_REFS",         CUSTOM_CTR,     ARCH_LLC_REFERENCE_EVTNR,                   ARCH_LLC_REFERENCE_UMASK,                   IA32_PERFEVTSEL6_ADDR,  IA32_PMC6,                      6},
    //{"L2_ALL_RFO",      CUSTOM_CTR,     HW_L2_RQSTS_ALL_RFO_EVTNR,                  HW_L2_RQSTS_ALL_RFO_UMASK,                  IA32_PERFEVTSEL7_ADDR,  IA32_PMC7,                      7},
    //{"L3_MISSES",       CUSTOM_CTR,     ARCH_LLC_MISS_EVTNR,                        ARCH_LLC_MISS_UMASK,                        IA32_PERFEVTSEL7_ADDR,  IA32_PMC7,                      7},
    { NULL,             0x0,            0x0,                                        0x0,                                        0x0,                    0x0,                            0}
};

#else

static struct counter_table_t counter_tbl[] = {
    /*
    {"L1_HITS",         CUSTOM_CTR,     MEM_LOAD_RETIRED_L1_HIT_EVTNR,      MEM_LOAD_RETIRED_L1_HIT_UMASK,  IA32_PERFEVTSEL0_ADDR,  IA32_PMC0,                       0},
    {"L1_MISSES",       CUSTOM_CTR,     MEM_LOAD_RETIRED_L1_MISS_EVTNR,     MEM_LOAD_RETIRED_L1_MISS_UMASK, IA32_PERFEVTSEL1_ADDR,  IA32_PMC1,                       1},
    */
    {"L2_LOAD_HITS",    CUSTOM_CTR,     MEM_LOAD_RETIRED_L2_HIT_EVTNR,      MEM_LOAD_RETIRED_L2_HIT_UMASK,  IA32_PERFEVTSEL0_ADDR,  IA32_PMC0,                       0},
    {"L2_LOAD_MISSES",  CUSTOM_CTR,     MEM_LOAD_RETIRED_L2_MISS_EVTNR,     MEM_LOAD_RETIRED_L2_MISS_UMASK, IA32_PERFEVTSEL1_ADDR,  IA32_PMC1,                       1},
    {"L2_REFS",         CUSTOM_CTR,     L2_RQSTS_REFERENCES_EVTNR,          L2_RQSTS_REFERENCES_UMASK,      IA32_PERFEVTSEL2_ADDR,  IA32_PMC2,                       2},
    {"L2_MISSES",       CUSTOM_CTR,     L2_RQSTS_MISSES_EVTNR,              L2_RQSTS_MISSES_UMASK,          IA32_PERFEVTSEL3_ADDR,  IA32_PMC3,                       3},
    {"L3_HITS",         CUSTOM_CTR,     ARCH_LLC_REFERENCE_EVTNR,           ARCH_LLC_REFERENCE_UMASK,       IA32_PERFEVTSEL4_ADDR,  IA32_PMC4,                       4},
    {"L3_MISSES",       CUSTOM_CTR,     ARCH_LLC_MISS_EVTNR,                ARCH_LLC_MISS_UMASK,            IA32_PERFEVTSEL5_ADDR,  IA32_PMC5,                       5},
    /*
    {"L3_HITS",         CUSTOM_CTR,     MEM_LOAD_RETIRED_L3_HIT_EVTNR,      MEM_LOAD_RETIRED_L3_HIT_UMASK,  IA32_PERFEVTSEL4_ADDR,  IA32_PMC4,                       4},
    {"L3_MISSES",       CUSTOM_CTR,     MEM_LOAD_RETIRED_L3_MISS_EVTNR,     MEM_LOAD_RETIRED_L3_MISS_UMASK, IA32_PERFEVTSEL5_ADDR,  IA32_PMC5,                       5},
    {"CPU_CYCLES",      FIXED_CTR,      0x0,                                0x0,                            IA32_CR_FIXED_CTR_CTRL, CPU_CLK_UNHALTED_THREAD_ADDR,    0x0},
    {"INSN_RETIRED",    FIXED_CTR,      0x0,                                0x0,                            IA32_CR_FIXED_CTR_CTRL, INST_RETIRED_ANY_ADDR,           0x0},
    */
    { NULL,             0x0,            0x0,                                0x0,                            0x0,                    0x0,                             0x0}
};
#endif

#endif // TYPES_H_
