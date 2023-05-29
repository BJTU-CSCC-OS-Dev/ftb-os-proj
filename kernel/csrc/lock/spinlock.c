#include "spinlock.h"

#include <define/riscv.h>
#include <lib/macro_builtin.h>
#include <lib/lib.h>
#include <proc/proc.h>


void spinlock_init(struct Spinlock * this, const char * name) {
	this->locked = false;
	this->name = name;
	this->cpuIdHoldingThis = NoneCpuId;
}

void spinlock_acquire(struct Spinlock * this) {
	cpu_push_intr_off();
	assert(!spinlock_held_by_this_cpu(this), this->name);
	loop_until (sync_test_and_set(&this->locked, 1));
	wall();
	this->cpuIdHoldingThis = cpu_get_this_id();
}

void spinlock_release(struct Spinlock * this) {
	assert(spinlock_held_by_this_cpu(this), this->name);
	this->cpuIdHoldingThis = NoneCpuId;
	wall();
	sync_reset(&this->locked);
	cpu_pop_intr_off();
}

bool spinlock_held_by_this_cpu(struct Spinlock * this) {
	CpuId_t thisCpuId = cpu_get_this_id();
	return this->locked && this->cpuIdHoldingThis == thisCpuId;
}

