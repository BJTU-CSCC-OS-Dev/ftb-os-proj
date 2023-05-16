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
// [KER_BASE, KMEM_BEGIN)  -- kernel data section and text section is directly mapped here
// [KMEM_BEGIN, KMEM_END) -- kmem range.

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
#define PLIC 0x0c000000ULL
#define PLIC_PRIORITY (PLIC + 0x0)
#define PLIC_PENDING (PLIC + 0x1000)
#define PLIC_MENABLE(hart) (PLIC + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart) (PLIC + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart) (PLIC + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart) (PLIC + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PLIC + 0x201004 + (hart)*0x2000)

// the kernel expects there to be RAM
// for use by the kernel and user pages
// from physical address 0x80000000 to KMEM_END
#define KER_BASE 0x80000000ULL
#define KMEM_END (KER_BASE + 128*1024*1024)

// map the trampoline page to the highest address,
// in both user and kernel space.
#define TRAMPOLINE (MAXVA - PGSIZE)

// map kernel stacks beneath the trampoline,
// each surrounded by invalid guard pages.
#define KER_STACK(p) (TRAMPOLINE - ((p)+1)* 2*PGSIZE)

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

extern char * KMEM_BEGIN;    //	from kernel.ld
