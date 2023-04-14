#pragma once

#include <define/types.h>

#define UART_FALSE ((size_t)(-1))
void uart_init();
size_t uart_poll_putch(char c);
size_t uart_poll_getch(void);

