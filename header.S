/* SPDX-License-Identifier: GPL-2.0 */
/*
 *	header.S
 *
 *	Copyright (C) 1991, 1992 Linus Torvalds
 *
 *	Based on bootsect.S and setup.S
 *	modified by more people than can be counted
 *
 *	Rewritten as a common file by H. Peter Anvin (Apr 2007)
 *
 * BIG FAT NOTE: We're in real mode using 64k segments.  Therefore segment
 * addresses must be multiplied by 16 to obtain their respective linear
 * addresses. To avoid confusion, linear addresses are written using leading
 * hex while segment addresses are written as segment:offset.
 *
 */

#include <asm/segment.h>
#include <asm/boot.h>
#include <asm/bootparam.h>
#include "boot.h"

BOOTSEG     = 0x07C0        /* original address of boot-sector */
SYSSEG		= 0x1000		/* historical load address >> 4 */

#ifndef SVGA_MODE
#define SVGA_MODE ASK_VGA
#endif

#ifndef ROOT_RDONLY
#define ROOT_RDONLY 1
#endif

    .code16
    .section ".bstext", "ax"

    .global bootsect_start
bootsect_start:
    # Normalize the start address
    ljmp    $BOOTSEG, $start2

start2:
    movw    %cs, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %ss
    xorw    %sp, %sp
    sti                 # 开中断
    cld                 # 清除方向标志

    movw    $bugger_off_msg, %si
msg_loop:
    lodsb               # 从%si搬移字符到%al
    andb    %al, %al    # 要显示的字符
    jz  bs_die          # 字符结束退出
    movb    $0xe, %ah   # 显示字符
    movw    $7, %bx     # 显示属性
    int $0x10           # 调用10H中断
    jmp msg_loop

bs_die:
	# Allow the user to press a key, then reboot
	xorw	%ax, %ax
	int	$0x16
	int	$0x19

	# int 0x19 should never return.  In case it does anyway,
	# invoke the BIOS reset code...
	ljmp	$0xf000,$0xfff0

	.section ".bsdata", "a"
bugger_off_msg:
	.ascii	"Use a boot loader.\r\n"
	.ascii	"\n"
	.ascii	"Remove disk and press any key to reboot...\r\n"
	.byte	0

    # Kernel attributes; used by setup.  This is part 1 of the
	# header, from the old boot sector.

	.section ".header", "a"
	.globl	sentinel
sentinel:	.byte 0xff, 0xff        /* Used to detect broken loaders */

	.globl	hdr
hdr:
setup_sects:	.byte 0  	/* Filled in by build.cc */
root_flags:	.word ROOT_RDONLY
syssize:	.long 0			/* Filled in by build.cc */
ram_size:	.word 0			/* Obsolete */
vid_mode:	.word SVGA_MODE
root_dev:	.word 0			/* Filled in by build.cc */
boot_flag:	.word 0xAA55

	# offset 512, entry point

	.globl	_start
_start:
		# Explicitly enter this as bytes, or the assembler
		# tries to generate a 3-byte jump here, which causes
		# everything else to push off to the wrong offset.
		.byte	0xeb		# short (2-byte) jump
		.byte	start_of_setup - 1f
1:

	# Part 2 of the header, from the old setup.S

		.ascii	"HdrS"		# header signature
		.word	0x0204		# header version number (>= 0x0105)
					# or else old loadlin-1.5 will fail)
		.globl realmode_swtch
realmode_swtch:	.word	0, 0		# default_switch, SETUPSEG
start_sys_seg:	.word	SYSSEG		# obsolete and meaningless, but just
					# in case something decided to "use" it
		.word	0 	# pointing to kernel version string
					# above section of header is compatible
					# with loadlin-1.5 (header v1.5). Don't
					# change it.

type_of_loader:	.byte	0		# 0 means ancient bootloader, newer
					# bootloaders know to change this.
					# See Documentation/x86/boot.rst for
					# assigned ids

# flags, unused bits must be zero (RFU) bit within loadflags
loadflags:
		.byte	LOADED_HIGH	# The kernel is to be loaded high

setup_move_size: .word  0x8000		# size to move, when setup is not
					# loaded at 0x90000. We will move setup
					# to 0x90000 then just before jumping
					# into the kernel. However, only the
					# loader knows how much data behind
					# us also needs to be loaded.

code32_start:		# here loaders can put a different
					# start address for 32-bit code.
		.long	0x100000	# 0x100000 = default for big kernel

ramdisk_image:	.long	0		# address of loaded ramdisk image
					# Here the loader puts the 32-bit
					# address where it loaded the image.
					# This only will be read by the kernel.

ramdisk_size:	.long	0		# its size in bytes

bootsect_kludge:
		.long	0		# obsolete

heap_end_ptr:	.word	_end+STACK_SIZE-512
					# (Header version 0x0201 or later)
					# space from here (exclusive) down to
					# end of setup code can be used by setup
					# for local heap purposes.

ext_loader_ver:
		.byte	0		# Extended boot loader version
ext_loader_type:
		.byte	0		# Extended boot loader type

cmd_line_ptr:	.long	0		# (Header version 0x0202 or later)
					# If nonzero, a 32-bit pointer
					# to the kernel command line.
					# The command line should be
					# located between the start of
					# setup and the end of low
					# memory (0xa0000), or it may
					# get overwritten before it
					# gets read.  If this field is
					# used, there is no longer
					# anything magical about the
					# 0x90000 segment; the setup
					# can be located anywhere in
					# low memory 0x10000 or higher.

initrd_addr_max: .long 0x7fffffff
					# (Header version 0x0203 or later)
					# The highest safe address for
					# the contents of an initrd
					# The current kernel allows up to 4 GB,
					# but leave it at 2 GB to avoid
					# possible bootloader bugs.

# End of setup header #####################################################

	.section ".entrytext", "ax"
start_of_setup:
# Force %es = %ds
	movw	%ds, %ax
	movw	%ax, %es
	cld

# Apparently some ancient versions of LILO invoked the kernel with %ss != %ds,
# which happened to work by accident for the old code.  Recalculate the stack
# pointer if %ss is invalid.  Otherwise leave it alone, LOADLIN sets up the
# stack behind its own code, so we can't blindly put it directly past the heap.

	movw	%ss, %dx
	cmpw	%ax, %dx	# %ds == %ss?
	movw	%sp, %dx
	je	2f		# -> assume %sp is reasonably set

	# Invalid %ss, make up a new stack
	movw	$_end, %dx
#ifdef CAN_USE_HEAP
	testb	$CAN_USE_HEAP, loadflags
	jz	1f
	movw	heap_end_ptr, %dx
#endif
1:	addw	$STACK_SIZE, %dx
	jnc	2f
	xorw	%dx, %dx	# Prevent wraparound

2:	# Now %dx should point to the end of our stack space
	andw	$~3, %dx	# dword align (might as well...)
	jnz	3f
	movw	$0xfffc, %dx	# Make sure we're not zero
3:	movw	%ax, %ss
	movzwl	%dx, %esp	# Clear upper half of %esp
	sti			# Now we should have a working stack

# We will have entered with %cs = %ds+0x20, normalize %cs so
# it is on par with the other segments.
	pushw	%ds
	pushw	$6f
	lretw
6:

# Check signature at end of setup
	cmpl	$0x5a5aaa55, setup_sig
	jne	setup_bad

# Zero the bss
	movw	$__bss_start, %di
	movw	$_end+3, %cx
	xorl	%eax, %eax
	subw	%di, %cx
	shrw	$2, %cx
	rep; stosl

# Jump to C code (should not return)
	calll	main

# Setup corrupt somehow...
setup_bad:
	movl	$setup_corrupt, %eax
	calll	puts
	# Fall through...

	.globl	die
	.type	die, @function
die:
	hlt
	jmp	die

	.size	die, .-die

	.section ".initdata", "a"
setup_corrupt:
	.byte	7
	.string	"No setup signature found...\n"
