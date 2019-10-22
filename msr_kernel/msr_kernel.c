#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>


void enable_l3_cache_miss(void);
void disable_l3_cache_miss(void);
void enable_global_counters(void);
void disable_global_counters(void);
unsigned long total_l3_cache_misses(void);



int __init PMC_init(void){
	printk(KERN_INFO "Inside the %s function.\n", __FUNCTION__);
	enable_global_counters();
	enable_l3_cache_miss();

	return 0;
}

void __exit PMC_exit(void){
	unsigned long total_misses = -1;

	printk(KERN_INFO "Inside the %s function.\n", __FUNCTION__);
	total_misses = total_l3_cache_misses();
	printk(KERN_INFO "Total L3 cache miss: %lu\n", total_misses);

	disable_l3_cache_miss();
	disable_global_counters();

}

void enable_l3_cache_miss(void){
	int reg_addr = 0x186; 		/* IA32_PERFEVTSELx MSRs start address */
	int event_num = 0x002e; 	/* L3 cache miss event number */
	int umask = 0x4100; 		/* L3 cache miss umask */
	int enable_bits = 0x430000; 	/* Enables user mode, OS mode, counters*/
	int event = enable_bits | umask | event_num;

	__asm__ ("wrmsr" : : "c"(reg_addr), "a"(event), "d"(0x00));
}

unsigned long total_l3_cache_misses(void){ 
	unsigned long total_misses;
	unsigned long eax_low, edx_high;
	int reg_addr = 0x0C1;		/* IA32_PMCx MSRs start address */

	__asm__("rdmsr" : "=a"(eax_low), "=d"(edx_high) : "c"(reg_addr));
	total_misses = ((long int)eax_low | (long int)edx_high<<32);



	return total_misses;
}

void enable_global_counters(void){
	int reg_addr = 0x38f;				/*  IA32_PERF_GLOBAL_CTRL start address */
	unsigned long enable_bits = 0x70000000f;	/*  IA32_PERF_GLOBAL_CTRL to enable the 3 fixed and 4 programmable counters */

	__asm__("wrmsr" : : "c"(reg_addr), "a"(enable_bits), "d"(0x00));
}

void disable_l3_cache_miss(void){
	int reg_addr_PEREVTSEL = 0x186;						/* IA32_PERFEVTSELx MSRs start address */
	int reg_addr_PMCx = 0x0C1;						/* IA32_PMCx MSRs start address */

	__asm__("wrmsr" : : "c"(reg_addr_PEREVTSEL), "a"(0x00), "d"(0x00));	/* Clears  IA32_PERFEVTSELx MSRs */
	__asm__("wrmsr" : : "c"(reg_addr_PMCx), "a"(0x00), "d"(0x00));		/* Clears counter */
}

void disable_global_counters(void){
	int reg_addr = 0x38f;						/*  IA32_PERF_GLOBAL_CTRL start address */		

	__asm__("wrmsr" : : "c"(reg_addr), "a"(0x00), "d"(0x00));	/* Clears IA32_PERF_GLOBAL_CTRL */
}

module_init(PMC_init);
module_exit(PMC_exit);
