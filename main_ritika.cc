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
#include "benchmarks.hh"

std::vector<uint8_t> cpus = {2}; // 0-indexed

void pin_thread(pthread_t thread, u_int16_t cpu) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);

    int rtn = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (rtn != 0) {
        fprintf(stderr, "could not pin thread: %s\n", strerror(errno));
    }
}

void configure_counters(MsrHandle * cpu_msr[],
                        std::vector<uint8_t> &cpus,
                        bool set) {
    uint32_t res;
    for (unsigned int i = 0; i < cpus.size(); ++i) {
        if (set) {
            printf("Register a new MSR handler for cpu %d\n", cpus[i]);
            cpu_msr[i] = new MsrHandle(cpus[i]);
        }
        assert(cpu_msr[i] != nullptr);

        /* Disable counters while programming */
        cpu_msr[i]->write(IA32_CR_PERF_GLOBAL_CTRL, 0);

        std::cout << "Resetting counters..." << std::endl;
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

        if (set) {
            std::cout << "Configuring counters..." << std::endl;
            /* Configure fixed registers */
            res = cpu_msr[i]->read(IA32_CR_FIXED_CTR_CTRL, &ctrl_reg.value);
            assert(res >= 0);
            uint64_t value = 0;
            if (N_FIXED_CTR > 0) {
                ctrl_reg.fields.os0 = 0;
                ctrl_reg.fields.usr0 = 1;
                ctrl_reg.fields.any_thread0 = 0;
                ctrl_reg.fields.enable_pmi0 = 0;
                value += (1ULL << 32);
            }
            if (N_FIXED_CTR > 1) {
                ctrl_reg.fields.os1 = 0;
                ctrl_reg.fields.usr1 = 1;
                ctrl_reg.fields.any_thread1 = 0;
                ctrl_reg.fields.enable_pmi1 = 0;
                value += (1ULL << 33);
            }
            if (N_FIXED_CTR > 2) {
                ctrl_reg.fields.os2 = 0;
                ctrl_reg.fields.usr2 = 1;
                ctrl_reg.fields.any_thread2 = 0;
                ctrl_reg.fields.enable_pmi2 = 0;
                value += (1ULL << 34);
            }
            cpu_msr[i]->write(IA32_CR_FIXED_CTR_CTRL, ctrl_reg.value);

            /* Configure PMC. We expect counter_tbl to declare PMC first and then fixed counters */
            for (int c = 0; c < N_CUSTOM_CTR; ++c) {
                if (c > PERF_MAX_CUSTOM_COUNTERS) {
                    break;
                }
                if (counter_tbl[c].ctr_type == CUSTOM_CTR) {
                    EventSelectRegister evt_reg;
                    res = cpu_msr[i]->read(counter_tbl[c].cfg_reg, &evt_reg.value);
                    assert (res >= 0);

                    evt_reg.value = 0;

                    evt_reg.fields.usr = 1;
                    evt_reg.fields.os = 0;
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

                    res = cpu_msr[i]->write(counter_tbl[c].cfg_reg, evt_reg.value);
                    assert (res >= 0);

                    value += (1ULL << counter_tbl[c].id);
                }
            }

            res = cpu_msr[i]->write(IA32_CR_PERF_GLOBAL_CTRL, value);
            assert(res >= 0);
        }
    }
}

/** Using this main function for some more testing */
int main(int argc, char *argv[]) 
{
    pin_thread(pthread_self(), 2);
    MsrHandle * cpu_msr[cpus.size()];

    /* Configure counters for each core */
    configure_counters(cpu_msr, cpus, true);

    bm_single_array(cpu_msr[0]);

    /* Clean MSRs */
    configure_counters(cpu_msr, cpus, false);

    for (unsigned int i = 0; i < cpus.size(); ++i) {
        delete cpu_msr[i];
    }
    return 0;
}
