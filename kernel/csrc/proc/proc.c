#include "proc.h"

#include <define/param.h>
#include <lib/lib.h>
#include <lock/sleeplock.h>
#include <mem/memlayout.h>


//	Cpu
struct Cpu cpus[MX_NCPU];


CpuId_t cpu_get_this_id() { return r_tp(); }

struct Cpu * cpu_get_this() {
	CpuId_t cid = cpu_get_this_id();
	struct Cpu * c = &cpus[cid];
	return c;
}

void cpu_push_intr_off() {
	u64 old = intr_get();
	intr_off();
	struct Cpu * c = cpu_get_this();
	if (c->depOfIntrOff == 0) {
		c->intrEnableBeforeOff = old;
	}
	++c->depOfIntrOff;
}

void cpu_pop_intr_off() {
	struct Cpu * c = cpu_get_this();
	assert(!intr_get(), "Intr enabled when calling this_cpu_pop_intr_off.");
	assert(c->depOfIntrOff >= 1, "Intr off depth is less than 1");
	c->depOfIntrOff -= 1;
	if (c->depOfIntrOff == 0 && c->intrEnableBeforeOff) {
		intr_on();
	}
}

//	KerProc
struct KerProc proc[MX_NPROC];
struct Spinlock pidLock, waitLock;
KerPid_t nxtPid;

struct KerProc * kerproc_get_this() {
	cpu_push_intr_off();
	struct Cpu * c = cpu_get_this();
	struct KerProc * p = c->proc;
	cpu_pop_intr_off();
	return p;
}

//	holding kernel proc lock and kernel proc status is changed.
//	This will change process status, so process lock should be held when calling this.
void kerproc_switch_to_scheduler() {
	struct KerProc * p = kerproc_get_this();
	struct Cpu * c = cpu_get_this();
	assert(spinlock_held_by_this_cpu(&p->lock), "kerproc_switch_to_scheduler but cpu is not holding lock");
	assert(c->depOfIntrOff == 1, "only one lock");
	assert(p->procStatus != proc_running, "schedule running kernel proc");
	assert(intr_get(), "");
	
	bool intrEnable = c->intrEnableBeforeOff;    //	this is the property of process not cpu
	swtch(&p->calleeSaveReg, &c->calleeSaveRegForScheduler);
	//	after switch back
	c->intrEnableBeforeOff = intrEnable;
}

void kerproc_sleep_on_chan(struct Spinlock * lock, void * chan) {
	struct KerProc * p = kerproc_get_this();
	
	spinlock_acquire(&p->lock);
	spinlock_release(lock);
	p->sleepingOnChan = chan;
	p->procStatus = proc_sleeping;
	
	kerproc_switch_to_scheduler();
	
	p->sleepingOnChan = nullptr;
	
	spinlock_release(&p->lock);
	spinlock_acquire(lock);
}

void kerproc_wakeup_on_chan(void * chan) {
	for (struct KerProc * p = proc; p != &proc[MX_NPROC]; ++p) {
		if (p != kerproc_get_this()) {
			spinlock_acquire(&p->lock);
			if (p->procStatus == proc_sleeping && p->sleepingOnChan == chan) {
				p->procStatus = proc_running;
			}
			spinlock_release(&p->lock);
		}
	}
}

void kerproc_init_all() {
	nxtPid = 1;
	spinlock_init(&pidLock, "pid_lock");
	spinlock_init(&waitLock, "wait_lock");
	for (struct KerProc * p = proc; p != &proc[MX_NPROC]; ++p) {
		spinlock_init(&p->lock, "proc_lock");
		p->kStack = KER_PROC_STACK_TOP_ADDR(p - proc);
	}
}
