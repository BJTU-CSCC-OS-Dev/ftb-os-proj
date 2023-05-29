#include "vm.h"

#include <lib/macro_builtin.h>
#include <lib/lib.h>
#include <define/riscv.h>
#include <mem/kernel_mem.h>
#include <mem/memlayout.h>
#include <trap/trap.h>
#include <define/param.h>


pagetable_t kernelPagetableRoot;

void vm_create_kernel_map(u64 va, u64 pa, u64 size, pte_t flag);

void vm_init_kernel_map() {
	kernelPagetableRoot = kmem_alloc();
	memset(kernelPagetableRoot, 0, PGSIZE);
	vm_create_kernel_map(UART0_ADDR, UART0_ADDR, PGSIZE, PTE_R | PTE_W);
	vm_create_kernel_map(VIRTIO0_ADDR, VIRTIO0_ADDR, PGSIZE, PTE_R | PTE_W);
	vm_create_kernel_map(PLIC_ADDR, PLIC_ADDR, PGSIZE, PTE_R | PTE_W);
	vm_create_kernel_map(KER_BASE, KER_BASE, (u64) (KER_TEXT_END) - KER_BASE, PTE_R | PTE_X);
	vm_create_kernel_map((u64) KER_TEXT_END, (u64) KER_TEXT_END, KMEM_END - (u64) KER_TEXT_END, PTE_R | PTE_W);
	vm_create_kernel_map(TRAMPOLINE, (u64) trampoline, PGSIZE, PTE_R | PTE_X);
	//	alloca a page for every kernel stack
	for (u64 i = 0; i < MX_NPROC; ++i) {
		u64 pa = (u64) kmem_alloc();
		assert(pa, "alloc failed");
		vm_create_kernel_map(KER_PROC_STACK_TOP_ADDR(i), pa, PGSIZE, PTE_R | PTE_W);
	}
}

void vm_init_kernel_map_for_every_cpu() {
	u64 r = MAKE_SATP(kernelPagetableRoot);
	w_satp(r);
	sfence_vma();
}

//	get the last level pte by walking. root should be created before.
pte_t * vm_walk(pagetable_t root, u64 va, bool alloc) {
	for (u64 level = 2; level != 0; --level) {
		pte_t * pte = &root[PX(level, va)];
		if (*pte & PTE_V) {
			root = (pagetable_t) PTE2PA(*pte);
		} else {
			if (!alloc || (root = (pagetable_t) kmem_alloc()) == nullptr) {
				return nullptr;
			}
			memset(root, 0, PGSIZE);
			*pte = PA2PTE(root) | PTE_V;
		}
	}
	return &root[PX(0, va)];
}

//	create disjoint tables
void vm_create_kernel_map(u64 va, u64 pa, u64 size, pte_t flag) {
	u64 a, last;
	pte_t * pte;
	a = PGROUNDDOWN(va);
	last = PGROUNDDOWN(va + size - 1);
	loop {
		pte = vm_walk(kernelPagetableRoot, a, true);
		assert(pte != nullptr, "pte is nullptr");
		assert(!(*pte & PTE_V), "remap");
		*pte = PA2PTE(pa) | flag | PTE_V;
		if (a == last) { break; }
		a += PGSIZE;
		pa += PGSIZE;
	};
}
