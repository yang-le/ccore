#pragma once

#include <linux/linkage.h>
#include <asm/boot.h>
#include <asm/bootparam.h>
#include <asm/io.h>
#include <asm/desc_defs.h>

#include "../ctype.h"

/* boot/compressed/vmlinux start and end markers */
extern char _head[], _end[];

/* misc.c */
extern struct boot_params *boot_params;
void __putstr(const char *s);
void __puthex(unsigned long value);
#define error_putstr(__x)  __putstr(__x)
#define error_puthex(__x)  __puthex(__x)

/* idt_64.c */
extern struct gate_desc boot_idt[BOOT_IDT_ENTRIES];
extern struct desc_ptr boot_idt_desc;

/* IDT Entry Points */
extern "C" void boot_page_fault(void);
