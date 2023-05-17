#pragma once

struct KmemPage {
	struct KmemPage * nxt;
};

void kmem_init();
void * kmem_alloc();
void kmem_free(struct KmemPage * p);
