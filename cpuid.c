#ifndef CPUID_H
#define CPUID_H

#include <stdio.h>
#include <iostream>

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

class CPUID {
  uint32_t regs[4];

public:
  explicit CPUID(unsigned i) {
    asm volatile
      ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
       : "a" (i), "c" (0));
    // ECX is set to zero for CPUID function 4
  }

  const uint32_t &EAX() const {return regs[0];}
  const uint32_t &EBX() const {return regs[1];}
  const uint32_t &ECX() const {return regs[2];}
  const uint32_t &EDX() const {return regs[3];}
};

uint32_t a[10];

int binaryNum[128];
void decToBinary(int n)
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

int main (int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Input CPUID eax value" << std::endl;
        exit(1);
    }

    uint32_t eax = atoi(argv[1]);
    std::cout << "EAX original value: " << eax << std::endl;

    CPUID cpuID(eax);
    std::cout << "EAX: ";
    decToBinary(cpuID.EAX());
    std::cout << " (" << std::string((const char *)&cpuID.EAX(), 4) << ")" <<std::endl;

    std::cout << "EBX: ";
    decToBinary(cpuID.EBX());
    std::cout << " (" << std::string((const char *)&cpuID.EBX(), 4) << ")" <<std::endl;

    std::cout << "ECX: ";
    decToBinary(cpuID.ECX());
    std::cout << " (" << std::string((const char *)&cpuID.ECX(), 4) << ")" <<std::endl;

    std::cout << "EDX: ";
    decToBinary(cpuID.EDX());
    std::cout << " (" << std::string((const char *)&cpuID.EDX(), 4) << ")" <<std::endl;

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
