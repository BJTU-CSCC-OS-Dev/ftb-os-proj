#include <define/riscv.h>
#include <device/uart.h>
#include <define/types.h>
#include <define/macro_builtin.h>


bool started = false;

void main() {
	u64 cpuId = r_mhartid();
	if (cpuId == 0) {
		/*	init process :
		 * 		console(TBD)
		 * 		kvm(Doing)
		 * 		S-Mode mmu(TBD)
		 */
		
		started = true;
	} else {
//		while (!started) {}
		//	disable other cores for now
		dead_loop();
		uart_init();
		uart_poll_putstr("Hello");
	}
	dead_loop();
}
