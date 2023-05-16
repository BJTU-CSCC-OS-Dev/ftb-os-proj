#include <define/riscv.h>
#include <device/uart.h>
#include <define/types.h>


bool started = false;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

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
		while (!started) {}
		uart_init();
		uart_poll_putstr("Hello");
	}
	while (1) {}
}

#pragma clang diagnostic pop