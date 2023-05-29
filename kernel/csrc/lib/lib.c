#include "lib.h"

#include <lib/macro_builtin.h>
#include <device/uart.h>


static inline void u64_to_str(u64 x, char * buf) {
	size_t i = 0;
	while (x) {
		buf[i] = x % 10;
		x /= 10;
		++i;
	}
	buf[i] = '\0';
	for (size_t j = 0; j < i / 2; ++j) {
		char ch = buf[j];
		buf[j] = buf[i - j - 1];
		buf[i - j - 1] = ch;
	}
}

void
assert_impl(bool cond, const char * condStr, const char * file, u64 line, const char * funcInfo, const char * info) {
	if (!cond) {
		uart_poll_putstr("assert ");
		uart_poll_putstr("[");
		uart_poll_putstr(condStr);
		uart_poll_putstr("] failed at [");
		uart_poll_putstr(file);
		uart_poll_putstr("::");
		uart_poll_put_u64(line);
		uart_poll_putstr(", ");
		uart_poll_putstr(funcInfo);
		uart_poll_putstr("] :");
		uart_poll_putstr(info);
		dead_loop();
	}
}

void panic(const char * info) {
	uart_poll_putstr("panic: ");
	uart_poll_putstr(info);
	dead_loop();
}

void memset(void * p, u8 byte, size_t size) {
	u8 * bp = p;
	for (size_t i = 0; i < size; ++i) {
		bp[i] = byte;
	}
}
