CC 				:= clang
CXX 			:= clang++
LD 				:= ld
OBJCOPY 		:= llvm-objcopy

QEMU 			:= qemu-system-x86_64.exe

# For gcc stack alignment is specified with -mpreferred-stack-boundary,
# clang has the option -mstack-alignment for that purpose.
ifneq ($(call cc-option, -mpreferred-stack-boundary=4),)
      cc_stack_align4 := -mpreferred-stack-boundary=2
      cc_stack_align8 := -mpreferred-stack-boundary=3
else ifneq ($(call cc-option, -mstack-alignment=16),)
      cc_stack_align4 := -mstack-alignment=4
      cc_stack_align8 := -mstack-alignment=8
endif

# How to compile the 16-bit code.  Note we always compile for -march=i386;
# that way we can complain to the user if the CPU is insufficient.
REALMODE_CFLAGS	:= -m16 -g -Os -DDISABLE_BRANCH_PROFILING -D__DISABLE_EXPORTS \
		   -Wall -Wstrict-prototypes -march=i386 -mregparm=3 \
		   -fno-strict-aliasing -fomit-frame-pointer -fno-pic \
		   -mno-mmx -mno-sse $(call cc-option,-fcf-protection=none)

REALMODE_CFLAGS += -ffreestanding
REALMODE_CFLAGS += -fno-stack-protector
REALMODE_CFLAGS += -Wno-address-of-packed-member
REALMODE_CFLAGS += $(cc_stack_align4)
REALMODE_CFLAGS += --target=x86_64-linux-gnu -fintegrated-as

CXXFLAGS 		:= $(REALMODE_CFLAGS) -I../include -D_SETUP
CXXFLAGS 		+= -fno-asynchronous-unwind-tables
CXXFLAGS 		+= -std=c++20

# If you want to preset the SVGA mode, uncomment the next line and
# set SVGA_MODE to whatever number you want.
# Set it to -DSVGA_MODE=NORMAL_VGA if you just want the EGA/VGA mode.
# The number is the same as you would ordinarily press at bootup.

SVGA_MODE	:= -DSVGA_MODE=NORMAL_VGA
ASFLAGS 	:= $(CXXFLAGS) $(SVGA_MODE) -D__ASSEMBLY__

setup-objs 	+= a20.o bioscall.o copy.o
setup-objs 	+= header.o main.o memory.o
setup-objs 	+= pm.o pmjump.o printf.o regs.o string.o tty.o video.o
setup-objs	+= video-mode.o

# The link order of the video-*.o modules can matter.  In particular,
# video-vga.o *must* be listed first, followed by video-vesa.o.
# Hardware-specific drivers should follow in the order they should be
# probed, and video-bios.o should typically be last.
setup-objs	+= video-vga.o
setup-objs	+= video-vesa.o
setup-objs	+= video-bios.o

bzImage: setup.bin vmlinux.bin tools/build
	tools/build setup.bin vmlinux.bin $@
	cat vmlinux.bin >> $@

setup.elf: setup.ld $(setup-objs)
	$(LD) -Map setup.map -o $@ -m elf_i386 --script $^

setup.bin: setup.elf
	$(OBJCOPY) -O binary $< $@

vmlinux.bin: compressed/vmlinux
	$(OBJCOPY) -O binary -R .note -R .comment -S $< $@

tools/build: tools/build.cc
	$(CXX) --std=c++20 -o $@ $<

compressed/vmlinux:
	$(MAKE) -C compressed vmlinux

qemu: bzImage
	$(QEMU) -kernel $<

clean:
	$(MAKE) -C compressed clean
	@-rm *.o *.elf *.bin *.map bzImage tools/build 2> /dev/null
