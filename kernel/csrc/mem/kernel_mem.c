#include "kernel_mem.h"


struct KmemPage {
	struct KmemPage * nxt;
};

void kmem_init() {
	//	kmem : [kernelMemEnd, PHYSTOP)
	
}
