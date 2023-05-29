#pragma once

#include <define/riscv.h>
#include <define/types.h>
#include <lock/spinlock.h>
#include <fs/file.h>
#include <define/param.h>
#include <proc/swtch.h>


enum ProcStatus {
	proc_unused,
	proc_used,
	proc_sleeping,
	proc_waiting,
	proc_running,
	proc_zombie,
};

struct TrapFrame {
	/*   0 */ u64 kernel_satp;   // kernel page table
	/*   8 */ u64 kernel_sp;     // top of process's kernel stack
	/*  16 */ u64 kernel_trap;   // usertrap()
	/*  24 */ u64 epc;           // saved user program counter
	/*  32 */ u64 kernel_hartid; // saved kernel tp
	/*  40 */ u64 ra;
	/*  48 */ u64 sp;
	/*  56 */ u64 gp;
	/*  64 */ u64 tp;
	/*  72 */ u64 t0;
	/*  80 */ u64 t1;
	/*  88 */ u64 t2;
	/*  96 */ u64 s0;
	/* 104 */ u64 s1;
	/* 112 */ u64 a0;
	/* 120 */ u64 a1;
	/* 128 */ u64 a2;
	/* 136 */ u64 a3;
	/* 144 */ u64 a4;
	/* 152 */ u64 a5;
	/* 160 */ u64 a6;
	/* 168 */ u64 a7;
	/* 176 */ u64 s2;
	/* 184 */ u64 s3;
	/* 192 */ u64 s4;
	/* 200 */ u64 s5;
	/* 208 */ u64 s6;
	/* 216 */ u64 s7;
	/* 224 */ u64 s8;
	/* 232 */ u64 s9;
	/* 240 */ u64 s10;
	/* 248 */ u64 s11;
	/* 256 */ u64 t3;
	/* 264 */ u64 t4;
	/* 272 */ u64 t5;
	/* 280 */ u64 t6;
};

//	Process Control Block
struct KerProc {
	struct Spinlock lock;
	enum ProcStatus procStatus;
	void * sleepingOnChan;
	bool killed;
	i32 exitStatus;
	KerPid_t pid;
	struct KerProc * parent;
	
	u64 kStack;
	u64 processMemSz;
	pagetable_t usrPagetable;
	struct TrapFrame * trapFrame;
	struct CalleeSaveReg calleeSaveReg;
	struct File * openedFiles[NOFILE];
};

void kerproc_init_all();
struct KerProc * kerproc_get_this();
void kerproc_switch_to_scheduler();
void kerproc_sleep_on_chan(struct Spinlock * lock, void * chan);
void kerproc_wakeup_on_chan(void * chan);

//	Cpu
struct Cpu {
	struct KerProc * proc;
	struct CalleeSaveReg calleeSaveRegForScheduler;
	u32 depOfIntrOff;
	bool intrEnableBeforeOff;
};

CpuId_t cpu_get_this_id();
struct Cpu * cpu_get_this();
void cpu_push_intr_off();
void cpu_pop_intr_off();