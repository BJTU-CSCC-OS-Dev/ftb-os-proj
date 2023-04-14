#	tool chain
ifndef toolPrefix
toolPrefix := $(shell if riscv64-unknown-elf-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-unknown-elf-'; \
	elif riscv64-linux-gnu-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-linux-gnu-'; \
	elif riscv64-unknown-linux-gnu-objdump -i 2>&1 | grep 'elf64-big' >/dev/null 2>&1; \
	then echo 'riscv64-unknown-linux-gnu-'; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find a riscv64 version of GCC/binutils." 1>&2; \
	echo "*** To turn off this error, run 'gmake toolPrefix= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif
cc = ${toolPrefix}gcc
as = ${toolPrefix}gas
ld = ${toolPrefix}ld
objcopy = ${toolPrefix}objcopy
objdump = ${toolPrefix}objdump

#	files var
kerCSrcRtPath = csrc
kerCSrcFile = $(patsubst ./%,%,$(shell find -name "*.c"))
kerSSrcRtPath = ssrc
kerSSrcFile = $(patsubst ./%,%,$(shell find -name "*.S"))
kerLdSrc = kernel.ld
kerBuildRtPath = build
kerObj += $(patsubst %.c,%.o,$(addprefix ${kerBuildRtPath}/,${kerCSrcFile}))
kerObj += $(patsubst %.S,%.o,$(addprefix ${kerBuildRtPath}/,${kerSSrcFile}))
kerMetaRtPath = meta
kerAsm += $(patsubst %.c,%.asm,$(addprefix ${kerMetaRtPath}/,${kerCSrcFile}))
kerAsm += $(patsubst %.S,%.asm,$(addprefix ${kerMetaRtPath}/,${kerSSrcFile}))

#	tool chain options
ccFlags = -Wall -Werror -O -fno-omit-frame-pointer -ggdb -gdwarf-2
ccFlags += -MD
ccFlags += -mcmodel=medany
ccFlags += -ffreestanding -fno-common -nostdlib -mno-relax
ccFlags += -I${kerCSrcRtPath}
ccFlags += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
# 		Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
ccFlags += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
ccFlags += -fno-pie -nopie
endif

ldFlags = -z max-page-size=4096

#	compile rules
${kerBuildRtPath}/${kerCSrcRtPath}/%.o: ${kerCSrcRtPath}/%.c
	mkdir -p $(dir $@)
	${cc} ${ccFlags} -c -o $@ $<

${kerBuildRtPath}/${kerSSrcRtPath}/%.o: ${kerSSrcRtPath}/%.S
	mkdir -p $(dir $@)
	${cc} -I${kerCSrcRtPath} -c -o $@ $<

${kerMetaRtPath}/${kerCSrcRtPath}/%.asm: ${kerBuildRtPath}/${kerCSrcRtPath}/%.o
	mkdir -p ${kerMetaRtPath}
	mkdir -p $(dir $@)
	${objdump} -S $< > $@

${kerMetaRtPath}/${kerSSrcRtPath}/%.asm: ${kerBuildRtPath}/${kerSSrcRtPath}/%.o
	mkdir -p ${kerMetaRtPath}
	mkdir -p $(dir $@)
	${objdump} -S $< > $@

kerExe = ${kerBuildRtPath}/kernel
${kerBuildRtPath}/kernel: ${kerObj} ${kerLdSrc}
	${ld} ${ldFlags} -T ${kerLdSrc} -o $@ ${kerObj}
	${objdump} -S $@ > ${kerMetaRtPath}/kernel.asm
	${objdump} -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > ${kerMetaRtPath}/kernel.sym

-include build/*.d

#	run options
runCfgFsImgPath := ../user/xv6-user-fs.img
runCfgNcpu := 1

#	qemu
qemu = qemu-system-riscv64
qemuOpts = -machine virt -bios none -kernel ${kerExe} -m 128M -smp ${runCfgNcpu} -nographic
qemuOpts += -global virtio-mmio.force-legacy=false
qemuOpts += -drive file=${runCfgFsImgPath},if=none,format=raw,id=x0
qemuOpts += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

qemu: ${kerExe}
	$(qemu) $(qemuOpts)

#	phony
.PHONY: show clean all
show:
	echo ${kerCSrcFile}
	echo ${kerObj}
clean:
	rm -rf ${kerBuildRtPath} ${kerMetaRtPath}
all: ${kerBuildRtPath}/kernel ${kerAsm}