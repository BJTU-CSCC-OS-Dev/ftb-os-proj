#pragma once
/*
 * spinlock define and methods
 */
#include <define/types.h>


struct Spinlock {
	bool locked;
	const char * name;
};


void spinlock_init(struct Spinlock * this, const char * name);
void spinlock_acquire(struct Spinlock * this);
void spinlock_release(struct Spinlock * this);