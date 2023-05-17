#pragma once

// Physical memory layout

// qemu -machine virt is set up like this,
// based on qemu's hw/riscv/virt.c:
//
// 00001000 -- boot ROM, provided by qemu
// 02000000 -- CLINT
// 0C000000 -- PLIC
// 10000000 -- uart0
// 10001000 -- virtio disk
// 80000000 -- boot ROM jumps here in machine mode
//          -- kernel loads the kernel here
// [KER_BASE, KER_TEXT_END    -- kernel text section
// [KER_TEXT_END, KMEM_BEGIN) -- kernel data section
// [KMEM_BEGIN, KMEM_END)     -- kmem range.

// qemu puts UART registers here in physical memory.
#define UART0_ADDR 0x10000000ULL
#define UART0_IRQ 10

// virtio mmio interface
#define VIRTIO0_ADDR 0x10001000ULL
#define VIRTIO0_IRQ 1

// core local interruptor (CLINT), which contains the timer.
//		mtime : Machine Time
//		mtimecmp : Machine Time Compare
#define CLINT_ADDR 0x2000000ULL
#define CLINT_MTIME_CMP_ADDR(hartId) (CLINT_ADDR + 0x4000 + 8*(hartId))
#define CLINT_MTIME_ADDR (CLINT_ADDR + 0xBFF8) // cycles since boot.

// qemu puts platform-level interrupt controller (PLIC) here.
#define PLIC_ADDR 0x0c000000ULL
#define PLIC_PRIORITY (PLIC_ADDR + 0x0)
#define PLIC_PENDING (PLIC_ADDR + 0x1000)
#define PLIC_MENABLE(hart) (PLIC_ADDR + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart) (PLIC_ADDR + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart) (PLIC_ADDR + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart) (PLIC_ADDR + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart) (PLIC_ADDR + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PLIC_ADDR + 0x201004 + (hart)*0x2000)

// the kernel expects there to be RAM
// for use by the kernel and user pages
// from physical address 0x80000000 to KMEM_END
#define KER_BASE 0x80000000ULL
//	KMEM_END % PGSIZE is always 0
#define KMEM_END (KER_BASE + 128*1024*1024)

// map the trampoline page to the highest address,
// in both user and kernel space.
#define TRAMPOLINE (MAXVA - PGSIZE)

//	top and bottom of kernel proc stack ([TOP, BOTTOM))
#define KER_PROC_STACK_TOP_ADDR(p) (TRAMPOLINE - ((p * 1ULL)+1) * 2 * PGSIZE)
#define KER_PROC_STACK_BOTTOM_ADDR(p) (KER_PROC_STACK_TOP_ADDR(p) + PGSIZE)

// User memory layout.
// Address zero first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
//   ...
//   TRAPFRAME (p->trapframe, used by the trampoline)
//   TRAMPOLINE (the same page as in the kernel)
#define TRAPFRAME (TRAMPOLINE - PGSIZE)

//	from kernel.ld
extern char KMEM_BEGIN[];
extern char KER_TEXT_END[];
