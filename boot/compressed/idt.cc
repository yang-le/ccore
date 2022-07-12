#include <asm/segment.h>
#include <asm/trapnr.h>
#include "misc.h"
#include "../string.h"

static void set_idt_entry(int vector, void (*handler)(void))
{
	unsigned long address = (unsigned long)handler;
	gate_desc entry;

	memset(&entry, 0, sizeof(entry));

	entry.offset_low    = (u16)(address & 0xffff);
	entry.segment       = __KERNEL_CS;
	entry.bits.type     = GATE_TRAP;
	entry.bits.p        = 1;
	entry.offset_middle = (u16)((address >> 16) & 0xffff);
	entry.offset_high   = (u32)(address >> 32);

	memcpy(&boot_idt[vector], &entry, sizeof(entry));
}

/* Have this here so we don't need to include <asm/desc.h> */
static void load_boot_idt(const struct desc_ptr *dtr)
{
	asm volatile("lidt %0"::"m" (*dtr));
}

/* Setup IDT before kernel jumping to  .Lrelocated */
extern "C" void load_stage1_idt(void)
{
	boot_idt_desc.address = (unsigned long)boot_idt;

	load_boot_idt(&boot_idt_desc);
}

extern "C" void load_stage2_idt(void)
{
	boot_idt_desc.address = (unsigned long)boot_idt;

	set_idt_entry(X86_TRAP_PF, boot_page_fault);

	load_boot_idt(&boot_idt_desc);
}

void cleanup_exception_handling(void)
{
	/* Set a null-idt, disabling #PF and #VC handling */
	boot_idt_desc.size    = 0;
	boot_idt_desc.address = 0;
	load_boot_idt(&boot_idt_desc);
}