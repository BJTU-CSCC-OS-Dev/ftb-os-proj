#include "define/types.h"
#include "define/param.h"
#include "mem/memlayout.h"
#include "define/riscv.h"


void main();

// entry.S needs one stack per CPU.
__attribute__ ((aligned (16))) char stack0[4096 * MX_NCPU];

// entry.S jumps here in machine mode on stack0.
void start() {
	// set M Previous Privilege mode to Supervisor, for mret.
	unsigned long x = r_mstatus();
	x &= ~MSTATUS_MPP_MASK;
	x |= MSTATUS_MPP_S;
	w_mstatus(x);
	
	// set M Exception Program Counter to main, for mret.
	// requires gcc -mcmodel=medany
	w_mepc((u64) main);
	
	// disable paging for now.
	w_satp(0);
	
	// delegate all interrupts and exceptions to supervisor mode.
	w_medeleg(0xffff);
	w_mideleg(0xffff);
	w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);
	
	// configure Physical Memory Protection to give supervisor mode
	// access to all of physical memory.
	w_pmpaddr0(0x3fffffffffffffull);
	w_pmpcfg0(0xf);
	
	// ask for clock interrupts.
//	timerinit();
	
	// keep each CPU's hartid in its tp register, for cpuid().
	u64 id = r_mhartid();
	w_tp(id);
	
	// switch to supervisor mode and jump to main().
	asm volatile("mret");
}
