CC 				:= clang
CXX 			:= clang++
LD 				:= ld
OBJCOPY 		:= objcopy

# CLANG_FLAGS must come before any cc-disable-warning or cc-option calls in
# case of cross compiling, as it has the '--target=' flag, which is needed to
# avoid errors with '-march=i386', and future flags may depend on the target to
# be valid.
CXXFLAGS		:= -m64 -O2 --target=x86_64-linux-gnu -fintegrated-as
CXXFLAGS		+= -fno-strict-aliasing
CXXFLAGS		+= -Wundef
CXXFLAGS		+= -DDISABLE_BRANCH_PROFILING
CXXFLAGS		+= -mcmodel=small -mno-red-zone
CXXFLAGS		+= -mno-mmx -mno-sse
CXXFLAGS		+= -ffreestanding -fshort-wchar
CXXFLAGS		+= -fno-stack-protector
CXXFLAGS		+= -Wno-pointer-sign
CXXFLAGS		+= -fno-asynchronous-unwind-tables
CXXFLAGS		+= -I../include -D__DISABLE_EXPORTS
CXXFLAGS 		+= -Wa,-mrelax-relocations=no
ASFLAGS  		:= $(CXXFLAGS) -D__ASSEMBLY__

vmlinux-objs := head_64.o

vmlinux: vmlinux.ld $(vmlinux-objs)
	$(LD) -Map vmlinux.map -o $@ -m elf_x86_64 --script $^

vmlinux.bin: vmlinux
	$(OBJCOPY) -R .comment -S $< $@

clean:
	rm *.o vmlinux *.bin *.map
