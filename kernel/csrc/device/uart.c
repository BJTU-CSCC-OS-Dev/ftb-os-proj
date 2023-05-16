#include "uart.h"

#include <define/memlayout.h>


#define Reg(reg) ((volatile unsigned char *)(UART0 + reg))

// the UART control registers.
// some have different meanings for
// read vs write.
// see http://byterunner.com/16550.html
#define RHR 0                 // receive holding register (for input bytes)
#define THR 0                 // transmit holding register (for output bytes)
#define IER 1                 // interrupt enable register
#define IER_RX_ENABLE (1<<0)
#define IER_TX_ENABLE (1<<1)
#define FCR 2                 // FIFO control register
#define FCR_FIFO_ENABLE (1<<0)
#define FCR_FIFO_CLEAR (3<<1) // clear the content of the two FIFOs
#define ISR 2                 // interrupt status register
#define LCR 3                 // line control register
#define LCR_EIGHT_BITS (3<<0)
#define LCR_BAUD_LATCH (1<<7) // special mode to set baud rate
#define LSR 5                 // line status register
#define LSR_RX_READY (1<<0)   // input is waiting to be read from RHR
#define LSR_TX_IDLE (1<<5)    // THR can accept another character to send

#define read_reg(reg) (*(Reg(reg)))
#define write_reg(reg, v) (*(Reg(reg)) = (v))

void uart_init() {
	write_reg(IER, 0x00);
	write_reg(LCR, LCR_BAUD_LATCH);
	write_reg(0, 0x03);
	write_reg(1, 0x00);
	write_reg(LCR, LCR_EIGHT_BITS);
	write_reg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);
}

size_t uart_poll_putch(char c) {
	while ((read_reg(LSR) & LSR_TX_IDLE) == 0) {}
	write_reg(THR, c);
	return 1;
}

size_t uart_poll_getch(void) {
	if (read_reg(LSR) & LSR_RX_READY) {
		return read_reg(RHR);
	} else {
		return UART_FALSE;
	}
}

size_t uart_poll_putstr(const char * str) {
	const char * p = str;
	size_t cnt = 0;
	while (*p) { cnt += uart_poll_putch(*(p++)); }
	return cnt;
}
