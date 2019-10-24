#ifndef CPUID_H
#define CPUID_H

#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <unistd.h>

/**
 * In this file we explore inline assembly to retrieve architectural information
 * through the CPUID instruction.
 *
 * It takes arguments in the eax register, and output in eax, ebx, ecx, and edx.
 *
 * The full description of the insn is located at page 752 of the complete Intel
 * programmers' manual (Vol. 2A 3-193)
 *
 * Notes about CPUID:
 * - On Intel 64 processors, CPUID clears the high 32 bits of the RAX/RBX/RCX/RDX registers in all modes.
 *
 *
 * eax = 0 // Genuine Intel
 * eax = 1 // Version information
 * eax = 2 // Cache and TLB info
 * eax = 0x8000000{2,3,4} // First part of brand string
 */


#include <stdint.h>
#include "msr.h"

class CPUID {
  uint32_t regs[4];

public:
  explicit CPUID(unsigned i, unsigned j) {
    asm volatile
      ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
       : "a" (i), "c" (j));
    // ECX is set to zero for CPUID function 4
  }

  const uint32_t &EAX() const {return regs[0];}
  const uint32_t &EBX() const {return regs[1];}
  const uint32_t &ECX() const {return regs[2];}
  const uint32_t &EDX() const {return regs[3];}
};

uint32_t a[10];

bool binaryNum[128];

bool is_little_endian()
{
    unsigned int i = 1;  
    char *c = (char*)&i;  
    if (*c)  
        return true;
    else
        return false;
}

void print_hex(uint32_t n)
{
   uint8_t* byte = (uint8_t*)(&n);

   if (is_little_endian())
   {
       for (int i = 3; i >= 0; i--)
       {
           printf("x%x ", *(byte + i));
       }
       printf("\n");
   }
   else
   {
       for (int i = 0; i < 4; i++)
       {
           printf("x%x ", *(byte + i));
       }
       printf("\n");
   }
}

void decToBinary(uint32_t n)
{
    // counter for binary array
    int i = 0;
    while (n > 0) {
        // storing remainder in binary array
        binaryNum[i] = n % 2;
        std::cout << binaryNum[i];
        n = n / 2;
        i++;
    }
    printf("\n");
}

bool are_msr_insns_enabled()
{
    CPUID cpuID(1, 0);

    if (cpuID.EDX() & (1 << 5))
        return true;
    return false;
}

void print_valid_byte(uint32_t reg)
{
    if (!(reg & (1 << 31)))
        print_hex(reg);
}

bool is_full_width_write_enabled()
{
    CPUID cpuID(1, 0);

    //decToBinary(cpuID.ECX());
    
    /** Check for support for IA32_PERF_CAPABILITIES MSR is provided by processor */
    msr msr_1(0);

    if (cpuID.ECX() & (1 << 15))
    {
        /** Only now check whether full-width writes are enabled or not */
        uint64_t perf_capability;
        bool ret = msr_1.msr_read(837, &perf_capability);
        printf("msr reg value: %lx\n", perf_capability);


        if (ret == 0)
        {
            /** Enabled if bit 13 is 1 */
            if (perf_capability & (1 << 13))
                printf("full width write capability enabled\n");
            else
                printf("full width write capability not enabled\n");
        }
    }
}

void get_cache_tlb_info()
{
    CPUID cpuID(2,0);
    uint8_t* valid_byte;
    uint32_t reg;

    printf("Valid byte descriptors:\n");
    
    /** The byte descriptors are valid if MSB of each register is 0 */
    printf("EAX:\n");
    reg = cpuID.EAX();
    print_valid_byte(reg);

    printf("EBX:\n");
    reg = cpuID.EBX();
    print_valid_byte(reg);

    printf("ECX:\n");
    reg = cpuID.ECX();
    print_valid_byte(reg);

    printf("EDX:\n");
    reg = cpuID.EDX();
    print_valid_byte(reg);

    printf("\nTo check what each byte represents check the description of each byte \nin each register from Table 3.12 on Page 790 of Intel Manual Vol 3\nLS byte of EAX should be ignored\n");
}

void get_deterministic_cache_params(int index)
{
    CPUID cpuID(4, index);
    std::cout << "EAX: ";
    print_hex(cpuID.EAX());

    std::cout << "EBX: ";
    decToBinary(cpuID.EBX());

    std::cout << "ECX: ";
    decToBinary(cpuID.ECX());

    std::cout << "EDX: ";
    decToBinary(cpuID.EDX());
}

void brandString(int eaxValues)
{
    if (eaxValues == 1) {
        __asm__("mov $0x80000002 , %eax\n\t");

    }
    else if (eaxValues == 2) {
        __asm__("mov $0x80000003 , %eax\n\t");

    }
    else if (eaxValues == 3) {
        __asm__("mov $0x80000004 , %eax\n\t");

    }
    __asm__("cpuid\n\t");
    __asm__("mov %%eax, %0\n\t":"=r" (a[0]));
    __asm__("mov %%ebx, %0\n\t":"=r" (a[1]));
    __asm__("mov %%ecx, %0\n\t":"=r" (a[2]));
    __asm__("mov %%edx, %0\n\t":"=r" (a[3]));
    printf("%s ", a);
}

void getCpuID()
{
    printf("Brand string is ");
    brandString(1);
    brandString(2);
    brandString(3);
    printf("\n");
}

void get_performance_counter_data()
{
    CPUID cpuID(10, 0);
    
    uint32_t eax = cpuID.EAX();
    uint32_t ebx = cpuID.EBX();

    printf("Performance counter monitoring data:\n");

    uint8_t* data = (uint8_t*)(&eax);
    if (is_little_endian())
    {
        printf("version ID: %x\n", *data);
        printf("No. of general purpose performance counters per logical core: %u\n", *(data + 1));
        printf("Bit width of performance monitoing counter : %u\n", *(data + 2));
        printf("no. of architectural performance monitoring events: %u\n", *(data + 3));

        data = (uint8_t*)(&ebx);
    }
    else
    {
        printf("version ID: %x\n", *(data + 3));
        printf("No. of general purpose performance counters per logical core: %u\n", *(data + 2));
        printf("Bit width of performance monitoing counter : %u\n", *(data + 1));
        printf("no. of architectural performance monitoring events: %u\n", *(data));

        data = (uint8_t*)(&ebx);
        data = data + 3;
    }

    printf("\n");

    if (*data & (1 << 0))
        printf("Core cycle event not avilable\n");
    else
        printf("Core cycle event avilable\n");

    if (*data & (1 << 1))
        printf("Instruction retired event not available\n");
    else
        printf("Instruction retired event available\n");

    if (*data & (1 << 2))
        printf("Reference cycle event not available\n");
    else
        printf("Reference cycle event available\n");

    if (*data & (1 << 3))
        printf("Last level cache refrence event available");
    else
        printf("Last level cache refrence event available");

    if (*data & (1 << 4))
        printf("Last level cache miss event not available\n");
    else
        printf("Last level cache miss event available\n");

    if (*data & (1 << 5))
        printf("Branch instruction retired event not available\n");
    else
        printf("Branch instruction retired event available\n");
    
    if (*data & (1 << 6))
        printf("Branch mispredict retired event not available\n");
    else
        printf("Branch mispredict retired event available\n");
}

    int aa[10000] = {0}, bb[10000] = {0}, sum[10000];
uint64_t count_l3_cache_misses(int cpu)
{
    msr miss(0);

    miss.enable_global_counters();
    miss.enable_l3_cache_miss();
    
    uint64_t l3_misses_start, l3_misses_end;
    
    l3_misses_start = miss.get_l3_cache_misses();

    /**
      * Code
      */
    for (int i = 0; i < 10000; i++)
    {
        sum[i] = aa[i] + bb[i];
    }

    l3_misses_end = miss.get_l3_cache_misses();
    printf("l3 cache misses - %lu\n", l3_misses_end - l3_misses_start);

    miss.disable_l3_cache_miss();
    miss.disable_global_counters();

    return l3_misses_end - l3_misses_start;
}

int main (int argc, char *argv[]) 
{    
    if (argc >= 2) 
    {
        uint32_t eax = atoi(argv[1]);
        std::cout << "EAX input value: " << eax << std::endl;
        uint32_t ecx = 0;
        if (argc > 2) {
            ecx = atoi(argv[2]);
        }

        CPUID cpuID(eax, ecx);

        printf("EAX: \n");
        decToBinary(cpuID.EAX());

        printf("EBX: \n");
        decToBinary(cpuID.EBX());

        printf("ECX: \n");
        decToBinary(cpuID.ECX());

        printf("EDX: \n");
        decToBinary(cpuID.EDX());
    }
    else
    {
//        if (are_msr_insns_enabled())
//            printf("RDMSR and WRMSR instructions are enabled\n");
//        else
//            printf("RDMSR and WRMSR instructions are not enabled\n");
// 
//        printf("Cache and TLB info: \n");
//        get_cache_tlb_info();
//
//        get_deterministic_cache_params(0);
//
//        get_performance_counter_data();
//         is_full_width_write_enabled();

        count_l3_cache_misses(0);
    }

    return 0;
}

/***** Some basics of inline assembly
 *
 * see https://ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
 int res=0;
 int op1=20;
 int op2=10;

asm("mov $10, %eax;"); //Basic inline assembly
asm("mov %eax, %ebx;"); // Extended (asm [volatile] ("(insn, src, dst)" : output operand : input operand: clobbers);
asm("mov %%ebx, %0": "=r"(res));
printf("%d\n", res);

Constraints for operand:
+---+--------------------+
| r |    Register(s)     |
+---+--------------------+
| a |   %eax, %ax, %al   |
| b |   %ebx, %bx, %bl   |
| c |   %ecx, %cx, %cl   |
| d |   %edx, %dx, %dl   |
| S |   %esi, %si        |
| D |   %edi, %di        |
+---+--------------------+

'=' means write-only constraint

In extended mode, there are two %’s prefixed to the register name, which helps GCC to distinguish between the operands and registers. operands have a single % as prefix.
*/

#endif // CPUID_H
