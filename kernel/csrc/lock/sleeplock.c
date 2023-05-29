#include "sleeplock.h"

#include <lib/lib.h>
#include <proc/proc.h>


void sleeplock_init(struct Sleeplock * this, const char * name) {
	this->locked = false;
	spinlock_init(&this->lock, "sleep lock");
	this->name = name;
	this->pid = NoneKerPid;
}

void sleeplock_acquire(struct Sleeplock * this) {
	spinlock_acquire(&this->lock);
	while (this->locked) {
		kerproc_sleep_on_chan(&this->lock, this);
	}
	this->locked = true;
	this->pid = kerproc_get_this()->pid;
	spinlock_release(&this->lock);
}

void sleeplock_release(struct Sleeplock * this) {
	spinlock_acquire(&this->lock);
	this->locked = false;
	this->pid = NoneKerPid;
	kerproc_wakeup_on_chan(this);
	spinlock_release(&this->lock);
}

bool sleeplock_held_by_this_cpu(struct Sleeplock * this) {
	spinlock_acquire(&this->lock);
	bool r = this->locked && (this->pid == kerproc_get_this()->pid);
	spinlock_release(&this->lock);
	return r;
}

