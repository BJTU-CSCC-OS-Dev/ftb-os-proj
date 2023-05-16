#include "lib.h"


void memset(void * p, u8 byte, size_t size) {
	u8 * bp = p;
	for (size_t i = 0; i < size; ++i) {
		bp[i] = byte;
	}
}
