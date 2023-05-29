#pragma once

#include <define/types.h>
#include <lock/spinlock.h>


struct Sleeplock {
	bool locked;
	struct Spinlock lock;
	const char * name;
	KerPid_t pid;
};

void sleeplock_init(struct Sleeplock * this, const char * name);
void sleeplock_acquire(struct Sleeplock * this);
void sleeplock_release(struct Sleeplock * this);
bool sleeplock_held_by_this_cpu(struct Sleeplock * this);