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
 */


int a[10];

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
    std::cout << std::endl;
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
    __asm__("xor %eax , %eax\n\t");
    __asm__("xor %ebx , %ebx\n\t");
    __asm__("xor %ecx , %ecx\n\t");
    __asm__("xor %edx , %edx\n\t");
    printf("Brand string is ");
    brandString(1);
    //brandString(2);
    //brandString(3);
    printf("\n");
}

int main () {
    /*
    __asm__("mov $0x0, %eax\n\t");
    __asm__("cpuid\n\t");
    __asm__("mov %%ebx, %0\n\t":"=r" (a[0]));
    __asm__("mov %%ecx, %0\n\t":"=r" (a[1]));
    __asm__("mov %%edx, %0\n\t":"=r" (a[2]));
    printf("%s\n", a);
    */

    getCpuID();
    //decToBinary(a[0]);

    /*
    int res=0;
    int op1=20;
    int op2=10;

    asm("mov $10, %eax;"); //Basic inline assembly
    asm("mov %eax, %ebx;"); // Extended (asm [volatile] ("(insn, src, dst)" : output operand : input operand: clobbers);
    asm("mov %%ebx, %0": "=r"(res));
    printf("%d\n", res);
    */

/*
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

    return 0;
}
