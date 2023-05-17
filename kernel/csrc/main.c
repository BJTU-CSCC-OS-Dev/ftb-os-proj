#include <define/riscv.h>
#include <device/uart.h>
#include <define/types.h>
#include <lib/macro_builtin.h>
#include <mem/kernel_mem.h>
#include <mem/vm.h>
#include <proc/proc.h>


bool started = false;

void main() {
	u64 cpuId = cpu_id();
	if (cpuId == 0) {
		/*	init process :
		 * 		console(TBD)
		 * 		kvm(Doing)
		 * 		S-Mode mmu(TBD)
		 */
		uart_init();
		kmem_init();
		vm_init_kernel_map();
		vm_inti_kernel_map_for_every_cpu();
		uart_poll_putstr("Hello");
		started = true;
	} else {
//		while (!started) {}
		//	disable other cores for now
		dead_loop();
		vm_inti_kernel_map_for_every_cpu();
		uart_poll_putstr("Hello");
	}
	dead_loop();
}
