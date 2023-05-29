#pragma once
/*
 * types definition
 */

typedef _Bool bool;
#define false ((bool)0)
#define true ((bool)1)
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef u32 size_t;
typedef unsigned long long u64;

typedef short i16;
typedef int i32;
typedef long long int i64;

//	Page table
typedef u64 pde_t;
typedef u64 pte_t;    //	page table entry
typedef pte_t * pagetable_t;

//	process
typedef u32 CpuId_t;
#define NoneCpuId ((CpuId_t)-1)
//typedef u32 UsrPid_t;
typedef u32 KerPid_t;
#define NoneKerPid ((KerPid_t)-1)

#define nullptr ((void *)0)

#define MV_BYTE_KMEM_INIT_VAL ((u8)(0x01))
#define MV_BYTE_KMEM_FREE_VAL ((u8)(0x02))