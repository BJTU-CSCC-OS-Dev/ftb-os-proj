#pragma once
#include <define/riscv.h>
#include <define/types.h>


static inline u64 cpu_id() { return r_tp(); }