#pragma once
/*
 * types definition
 */

typedef unsigned char bool;
#define false 0
#define true 1
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef u32 size_t;
typedef unsigned long long u64;

typedef short i16;
typedef int i32;
typedef long long int i64;

typedef u64 pde_t;

#define nullptr ((void *)0)

#define MV_BYTE_KMEM_INIT_VAL ((u8)(0x01))
#define MV_BYTE_KMEM_FREE_VAL ((u8)(0x02))