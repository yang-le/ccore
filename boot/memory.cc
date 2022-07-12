/*
 * Memory detection code
 */

#include "boot.h"

#define SMAP	0x534d4150	/* ASCII "SMAP" */

static void detect_memory_e820(void)
{
    int count = 0;
    biosregs ireg, oreg;
    boot_e820_entry *desc = boot_params.e820_table;
    boot_e820_entry buf = {0};

    initregs(ireg);
	ireg.ax  = 0xe820;
	ireg.cx  = sizeof(buf);
	ireg.edx = SMAP;
	ireg.di  = (size_t)&buf;

    do {
        intcall(0x15, &ireg, &oreg);
		ireg.ebx = oreg.ebx; /* for next iteration... */

        if (oreg.eflags & X86_EFLAGS_CF)
            break;
        
        if (oreg.eax != SMAP) {
            count = 0;
            break;
        }

        *desc++ = buf;
        count++;
    } while(ireg.ebx && count < ARRAY_SIZE(boot_params.e820_table));

    boot_params.e820_entries = count;
}

void detect_memory(void)
{
	detect_memory_e820();
}
