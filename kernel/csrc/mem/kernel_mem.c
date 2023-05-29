#include "kernel_mem.h"

#include <define/types.h>
#include <lib/lib.h>
#include <define/riscv.h>
#include <device/uart.h>
#include <lock/spinlock.h>
#include <mem/memlayout.h>


struct KmemPage kmemPageHead;//	kmemPageHead->nxt is the first
struct Spinlock kmemLock;    //	protect kmemPageHead
u64 kmem_count;

void kmem_init() {
	spinlock_init(&kmemLock, "kmemLock");
	kmem_count = 0;
	//	kmem : [KMEM_BEGIN, KMEM_STOP)
	kmemPageHead.nxt = nullptr;
	u64 addrBegin = PGROUNDUP(KMEM_BEGIN);
	u64 addrEnd = KMEM_END;    //	note : KMEM_END is always page aligned
	for (u64 addr = addrBegin; addr != addrEnd; addr += PGSIZE) {
//		uart_poll_put_u64(addr);
//		uart_poll_putch('\n');
		kmem_free((struct KmemPage *) addr);
	}
}

void * kmem_alloc() {
	spinlock_acquire(&kmemLock);
	struct KmemPage * p = kmemPageHead.nxt;
	if (p) {
		kmemPageHead.nxt = p->nxt;
		--kmem_count;
	}
//	uart_poll_putstr("kmem_alloc : ");
//	uart_poll_put_u64(kmem_count);
//	uart_poll_putstr("\n");
	spinlock_release(&kmemLock);
	if (p) {
		memset(p, MV_BYTE_KMEM_INIT_VAL, PGSIZE);
	}
	return p;
}

void kmem_free(struct KmemPage * p) {
	memset(p, MV_BYTE_KMEM_FREE_VAL, PGSIZE);
	spinlock_acquire(&kmemLock);
	p->nxt = kmemPageHead.nxt;
	kmemPageHead.nxt = p;
	++kmem_count;
//	uart_poll_putstr("kmem_free : ");
//	uart_poll_put_u64(kmem_count);
//	uart_poll_putstr("\n");
	spinlock_release(&kmemLock);
}
