#include "spinlock.h"

#include <define/riscv.h>
#include "lib/macro_builtin.h"


void spinlock_init(struct Spinlock * this, const char * name) {
	this->locked = false;
	this->name = name;
}

void spinlock_acquire(struct Spinlock * this) {
	loop_until (sync_test_and_set(&this->locked, 1));
	wall();
}

void spinlock_release(struct Spinlock * this) {
	wall();
	sync_reset(&this->locked);
}
