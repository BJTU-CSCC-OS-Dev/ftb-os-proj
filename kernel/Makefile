# 	macro
define mkdir_and_touch
	mkdir -p $1
	touch $1
endef

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
kerEntryO = ${kerBuildRtPath}/${kerSSrcRtPath}/entry.o
kerObj := ${kerEntryO} $(filter-out ${kerEntryO},${kerObj})
kerMetaRtPath = meta
kerAsm += $(patsubst %.c,%.asm,$(addprefix ${kerMetaRtPath}/,${kerCSrcFile}))
kerAsm += $(patsubst %.S,%.asm,$(addprefix ${kerMetaRtPath}/,${kerSSrcFile}))

#	tool chain options
ccFlags =  -Wall -Werror
ccFlags += -O -fno-omit-frame-pointer -ggdb -gdwarf-2 -std=c11
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

#	Rt folders
${kerBuildRtPath}:
	$(call mkdir_and_touch, $@)
${kerMetaRtPath}:
	$(call mkdir_and_touch, $@)

#	compile rules
${kerBuildRtPath}/${kerCSrcRtPath}/%.o: ${kerCSrcRtPath}/%.c | ${kerBuildRtPath}
	$(call mkdir_and_touch, $(dir $@))
	${cc} ${ccFlags} -c -o $@ $<

${kerBuildRtPath}/${kerSSrcRtPath}/%.o: ${kerSSrcRtPath}/%.S | ${kerBuildRtPath}
	$(call mkdir_and_touch, $(dir $@))
	${cc} -I${kerCSrcRtPath} -c -o $@ $<

${kerMetaRtPath}/${kerCSrcRtPath}/%.asm: ${kerBuildRtPath}/${kerCSrcRtPath}/%.o | ${kerMetaRtPath}
	$(call mkdir_and_touch, $(dir $@))
	${objdump} -S $< > $@

${kerMetaRtPath}/${kerSSrcRtPath}/%.asm: ${kerBuildRtPath}/${kerSSrcRtPath}/%.o | ${kerMetaRtPath}
	$(call mkdir_and_touch, $(dir $@))
	${objdump} -S $< > $@

kerExe = ${kerBuildRtPath}/kernel
${kerExe}: ${kerObj} ${kerLdSrc} ${kerBuildRtPath} | ${kerMetaRtPath}
	${ld} ${ldFlags} -T ${kerLdSrc} -o $@ ${kerObj}
	${objdump} -S $@ > ${kerMetaRtPath}/kernel.asm

kerSym = ${kerMetaRtPath}/kernel.sym
${kerSym}: ${kerExe} | ${kerMetaRtPath}
	${objdump} -t $< | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $@

-include build/*.d

#	run options
runCfgFsImgPath := ../user/xv6-user-fs.img
runCfgNcpu := 1 #	qemu
qemu = qemu-system-riscv64
qemuOpts = -machine virt -bios none -kernel ${kerExe} -m 128M -smp ${runCfgNcpu} -nographic
qemuOpts += -global virtio-mmio.force-legacy=false
qemuOpts += -drive file=${runCfgFsImgPath},if=none,format=raw,id=x0
qemuOpts += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

gdbPort = $(shell expr `id -u` % 5000 + 25000)
qemuGdb = $(shell if $(qemu) -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(gdbPort)"; \
	else echo "-s -p $(gdbPort)"; fi)

.PHONY: qemu qemu-gdb
qemu: ${kerExe}
	$(qemu) $(qemuOpts)

.gdbinit: .gdbinit.tmpl-riscv
	sed "s/:1234/:$(gdbPort)/" < $^ > $@

qemu-gdb: ${kerExe} .gdbinit
	@echo "*** Now run 'gdb' in another window." 1>&2
	$(qemu) $(qemuOpts) -S $(qemuGdb)

#	helper phony
.PHONY: show clean all
show:
	$(info ${kerObj})
	$(info ${kerExe})
	$(info ${kerAsm})
clean:
	rm -rf ${kerBuildRtPath} ${kerMetaRtPath}
all: ${kerExe} ${kerAsm} ${kerSym}
