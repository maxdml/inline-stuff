#include "msr-old.h"

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

msr::msr(uint8_t cpu)
{
	char msr_file_name[64];
	sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
	fd = open(msr_file_name, O_RDWR);
	if (fd < 0) 
    {
		if (errno == ENXIO) 
        {
			fprintf(stderr, "rdmsr: No CPU %d\n", cpu);
		} 
        else if (errno == EIO) 
        {
			fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n",
				cpu);
		} 
        else 
        {
			perror("rdmsr: open");
		}
	}
}

bool msr::msr_read(uint64_t reg, uint64_t *data)
{
	if (pread(fd, data, sizeof data, reg) != sizeof data) 
    {
		if (errno == EIO) 
        {
			fprintf(stderr, "rdmsr: cannot read "
				"MSR 0x%016lx\n",
				 reg);
			return 1;
		} 
        else 
        {
			perror("rdmsr: pread");
			return 1;
		}
	}
    return 0;
}

bool msr::msr_write(uint64_t reg, uint64_t* data)
{
    if (pwrite(fd, data, sizeof data, reg) != sizeof data) 
    {
        if (errno == EIO) 
        {
            fprintf(stderr,
                    "wrmsr: cannot set MSR 0x%016lx\n",
                    reg);
            return 1;
        } 
        else 
        {
            printf("Error in writing : %lx\n", reg);
            perror("wrmsr: pwrite");
            return 1;
        }
    }
    return 0;
}

msr::~msr()
{
    close(fd);
}

void msr::enable_l3_cache_miss()
{
	uint64_t reg = 0x186; 		/* IA32_PERFEVTSELx MSRs start address */
	uint64_t  event_num = 0x002e; 	/* L3 cache miss event number */
	uint64_t  umask = 0x4100; 		/* L3 cache miss umask */
	uint64_t  enable_bits = 0x430000; 	/* Enables user mode, OS mode, counters*/
	uint64_t  event = enable_bits | umask | event_num;

    msr_write(reg, &event);
}

void msr::enable_l3_cache_reference()
{
	uint64_t reg = 0x187; 		/* IA32_PERFEVTSELx MSRs start address */
	uint64_t  event_num = 0x002e; 	/* L3 cache miss event number */
	uint64_t  umask = 0x4f00; 		/* L3 cache miss umask */
	uint64_t  enable_bits = 0x430000; 	/* Enables user mode, OS mode, counters*/
	uint64_t  event = enable_bits | umask | event_num;

    msr_write(reg, &event);
}

void msr::enable_global_counters()
{
	uint64_t reg = 0x38f;				/*  IA32_PERF_GLOBAL_CTRL start address */
	uint64_t enable_bits = 0x0f;	/*  IA32_PERF_GLOBAL_CTRL to enable the 4 programmable counters */

    msr_write(reg, &enable_bits);
}

void msr::disable_l3_cache_miss()
{
	uint64_t reg_PEREVTSEL = 0x186;						/* IA32_PERFEVTSELx MSRs start address */
	uint64_t reg_PMCx = 0x0C1;						/* IA32_PMCx MSRs start address */
    uint64_t val = 0x00;

    msr_write(reg_PEREVTSEL, &val);
    msr_write(reg_PMCx, &val);
}

void msr::disable_l3_cache_reference()
{
	uint64_t reg_PEREVTSEL = 0x187;						/* IA32_PERFEVTSELx MSRs start address */
	uint64_t reg_PMCx = 0x0C2;						/* IA32_PMCx MSRs start address */
    uint64_t val = 0x00;

    msr_write(reg_PEREVTSEL, &val);
    msr_write(reg_PMCx, &val);
}

void msr::disable_global_counters()
{
	uint64_t reg = 0x38f;						/*  IA32_PERF_GLOBAL_CTRL start address */		
    uint64_t val = 0x00;

    msr_write(reg, &val);
}

uint64_t msr::get_l3_cache_misses()
{
	uint64_t total_misses;
	uint64_t reg = 0x0C1;		/* IA32_PMC0 MSR address */

    msr_read(reg, &total_misses);

	return total_misses;

}

uint64_t msr::get_l3_cache_references()
{
	uint64_t total_references;
	uint64_t reg = 0x0C2;		/* IA32_PMC1 MSR address */

    msr_read(reg, &total_references);

	return total_references;

}
