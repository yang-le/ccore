#include "error.h"
#include "misc.h"

#include <linux/math.h>
#include <asm/trap_pf.h>
#include <asm/trapnr.h>
#include <asm/pgtable_types.h>
#include <asm/special_insns.h>

struct pt_regs {
/*
 * C ABI says these regs are callee-preserved. They aren't saved on kernel entry
 * unless syscall needs a complete, fully filled "struct pt_regs".
 */
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long bp;
	unsigned long bx;
/* These regs are callee-clobbered. Always saved on kernel entry. */
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long ax;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;
/*
 * On syscall entry, this is syscall#. On CPU exception, this is error code.
 * On hw interrupt, it's IRQ number:
 */
	unsigned long orig_ax;
/* Return frame for iretq */
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long sp;
	unsigned long ss;
/* top of stack page */
};

/*
 * Adds the specified range to the identity mappings.
 */
static void add_identity_map(unsigned long start, unsigned long end)
{
	int ret;

	/* Align boundary to 2M. */
	start = round_down(start, PMD_SIZE);
	end = round_up(end, PMD_SIZE);
	if (start >= end)
		return;

	/* TODO: Build the mapping. */
	// ret = kernel_ident_mapping_init(&mapping_info, (pgd_t *)top_level_pgt, start, end);
	// if (ret)
		error("Error: kernel_ident_mapping_init() failed\n");
}

/* Locates and clears a region for a new top level page table. */
extern "C" void initialize_identity_maps(void *rmode)
{
    __putstr("initialize_identity_maps\n");
}

static void do_pf_error(const char *msg, unsigned long error_code,
			unsigned long address, unsigned long ip)
{
	error_putstr(msg);

	error_putstr("\nError Code: ");
	error_puthex(error_code);
	error_putstr("\nCR2: 0x");
	error_puthex(address);
	error_putstr("\nRIP relative to _head: 0x");
	error_puthex(ip - (unsigned long)_head);
	error_putstr("\n");

	error("Stopping.\n");
}

extern "C" void do_boot_page_fault(struct pt_regs *regs, unsigned long error_code)
{
	unsigned long address = native_read_cr2();
	unsigned long end;

	address   &= PMD_MASK;
	end        = address + PMD_SIZE;

	/*
	 * Check for unexpected error codes. Unexpected are:
	 *	- Faults on present pages
	 *	- User faults
	 *	- Reserved bits set
	 */
	if (error_code & (X86_PF_PROT | X86_PF_USER | X86_PF_RSVD))
		do_pf_error("Unexpected page-fault:", error_code, address, regs->ip);

	/*
	 * Error code is sane - now identity map the 2M region around
	 * the faulting address.
	 */
	add_identity_map(address, end);
}
