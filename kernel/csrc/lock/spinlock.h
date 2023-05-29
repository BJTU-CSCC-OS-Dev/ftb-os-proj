#pragma once
/*
 * spinlock define and methods
 */
#include <define/types.h>


struct Spinlock {
	bool locked;
	const char * name;
	CpuId_t cpuIdHoldingThis;
};


void spinlock_init(struct Spinlock * this, const char * name);
void spinlock_acquire(struct Spinlock * this);
void spinlock_release(struct Spinlock * this);
bool spinlock_held_by_this_cpu(struct Spinlock * this);