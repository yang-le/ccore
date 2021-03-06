/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Early IDT handler entry points
 *
 * Copyright (C) 2019 SUSE
 *
 * Author: Joerg Roedel <jroedel@suse.de>
 */

#include <asm/segment.h>
#include <linux/linkage.h>

.macro EXCEPTION_HANDLER name function error_code=0
SYM_FUNC_START(\name)

	/* Build pt_regs */
	.if \error_code == 0
	pushq   $0
	.endif

	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %rax
	pushq   %r8
	pushq   %r9
	pushq   %r10
	pushq   %r11
	pushq   %rbx
	pushq   %rbp
	pushq   %r12
	pushq   %r13
	pushq   %r14
	pushq   %r15

	/* Call handler with pt_regs */
	movq    %rsp, %rdi
	/* Error code is second parameter */
	movq	120(%rsp), %rsi
	call    \function

	/* Restore regs */
	popq    %r15
	popq    %r14
	popq    %r13
	popq    %r12
	popq    %rbp
	popq    %rbx
	popq    %r11
	popq    %r10
	popq    %r9
	popq    %r8
	popq    %rax
	popq    %rcx
	popq    %rdx
	popq    %rsi
	popq    %rdi

	/* Remove error code and return */
	addq    $8, %rsp

	iretq
SYM_FUNC_END(\name)
	.endm

	.text
	.code64

EXCEPTION_HANDLER	boot_page_fault do_boot_page_fault error_code=1
