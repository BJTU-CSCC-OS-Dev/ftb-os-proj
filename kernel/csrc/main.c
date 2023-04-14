#include <device/uart.h>


void main() {
	uart_init();
	uart_poll_putch('h');
	
	while (1) {}
}