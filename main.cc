#include "boot.h"

#define OLD_CL_MAGIC		0xA33F
#define OLD_CL_ADDRESS		0x020	/* Relative to real mode data */

alignas(16)
struct boot_params boot_params;

char *HEAP = _end;
char *heap_end = _end;		/* Default end of heap = no heap */

/*
 * Copy the header into the boot parameter block.  Since this
 * screws up the old-style command line protocol, adjust by
 * filling in the new-style command line pointer instead.
 */

static void copy_boot_params(void)
{
    struct old_cmdline {
		u16 cl_magic;
		u16 cl_offset;
	};
	const struct old_cmdline * const oldcmd =
		(const struct old_cmdline *)OLD_CL_ADDRESS;

	static_assert(sizeof(boot_params) == 4096);
	memcpy(&boot_params.hdr, &hdr, sizeof(hdr));

    if (!boot_params.hdr.cmd_line_ptr &&
	    oldcmd->cl_magic == OLD_CL_MAGIC) {
		/* Old-style command line protocol. */
		u16 cmdline_seg;

		/* Figure out if the command line falls in the region
		   of memory that an old kernel would have copied up
		   to 0x90000... */
		if (oldcmd->cl_offset < boot_params.hdr.setup_move_size)
			cmdline_seg = ds();
		else
			cmdline_seg = 0x9000;

		boot_params.hdr.cmd_line_ptr =
			(cmdline_seg << 4) + oldcmd->cl_offset;
	}
}

/*
 * Query the keyboard lock status as given by the BIOS, and
 * set the keyboard repeat rate to maximum.  Unclear why the latter
 * is done here; this might be possible to kill off as stale code.
 */
static void keyboard_init(void)
{
	struct biosregs ireg, oreg;
	initregs(ireg);

	ireg.ah = 0x02;		/* Get keyboard status */
	intcall(0x16, &ireg, &oreg);
	boot_params.kbd_status = oreg.al;

	ireg.ax = 0x0305;	/* Set keyboard repeat rate */
	intcall(0x16, &ireg, nullptr);
}

/*
    In order for the firmware built into the system to optimize itself for running in Long Mode,
    AMD recommends that the OS notify the BIOS about the intended target environment that the OS will be running in: 32-bit mode, 64-bit mode, or a mixture of both modes.
    This can be done by calling the BIOS interrupt 15h from Real Mode with AX set to 0xEC00, and BL set to 1 for 32-bit Protected Mode, 2 for 64-bit Long Mode, or 3 if both modes will be used.
*/
/*
 * Tell the BIOS what CPU mode we intend to run in.
 */
static void set_bios_mode(void)
{
	struct biosregs ireg;

	initregs(ireg);
	ireg.ax = 0xec00;
	ireg.bx = 2;
	intcall(0x15, &ireg, nullptr);
}

static void init_heap(void)
{
	char *stack_end;

	if (boot_params.hdr.loadflags & CAN_USE_HEAP) {
		asm("leal %P1(%%esp),%0"
		    : "=r" (stack_end) : "i" (-STACK_SIZE));

		heap_end = (char *)
			((size_t)boot_params.hdr.heap_end_ptr + 0x200);
		if (heap_end > stack_end)
			heap_end = stack_end;
		
		// printf("stack_end %x, heap_end %x after init_heap\n",
		// 	stack_end, heap_end);
	} else {
		/* Boot protocol 2.00 only, no heap available */
		puts("WARNING: Ancient bootloader, some functionality "
		     "may be limited!\n");
	}
}

void debug_print_boot_params(void)
{
	printf("boot_params:\n"
		"setup_sects %d\n"
		"root_flags %x\n"
		"syssize %d\n"
		"ram_size %d\n"
		"vid_mod %x\n"
		"root_dev %d\n"
		"boot_flag %x\n"
		"jump %x\n"
		"header %x\n"
		"version %x\n"
		"realmode_swtch %x\n"
		"start_sys_seg %x\n"
		"kernel_version %x\n"
		"type_of_loader %x\n"
		"loadflags %x\n"
		"setup_move_size %x\n"
		"code32_start %x\n"
		"ramdisk_iamge %x\n"
		"ramdisk_size %x\n"
		"bootsect_kludge %x\n"
		"heap_end_ptr %x \n"
		"ext_loader_ver %x \n"
		"ext_loader_type %x\n"
		"cmd_line_ptr %x\n"
		"initrd_addr_max %x\n",
		boot_params.hdr.setup_sects,
		boot_params.hdr.root_flags,
		boot_params.hdr.syssize,
		boot_params.hdr.ram_size,
		boot_params.hdr.vid_mode,
		boot_params.hdr.root_dev,
		boot_params.hdr.boot_flag,
		boot_params.hdr.jump,
		boot_params.hdr.header,
		boot_params.hdr.version,
		boot_params.hdr.realmode_swtch,
		boot_params.hdr.start_sys_seg,
		boot_params.hdr.kernel_version,
		boot_params.hdr.type_of_loader,
		boot_params.hdr.loadflags,
		boot_params.hdr.setup_move_size,
		boot_params.hdr.code32_start,
		boot_params.hdr.ramdisk_image,
		boot_params.hdr.ramdisk_size,
		boot_params.hdr.bootsect_kludge,
		boot_params.hdr.heap_end_ptr,
		boot_params.hdr.ext_loader_ver,
		boot_params.hdr.ext_loader_type,
		boot_params.hdr.cmd_line_ptr,
		boot_params.hdr.initrd_addr_max
	);
}

void debug_print_memory_layout(void)
{
	const char* addr_range_type[] = {
        "Available",
        "Reserved",
        "ACPI Reclaim",
        "ACPI NVS"
    };

    printf("Memory Info:\n%-16s %-16s %s\n", "addr", "size", "type");
    for (int i = 0; i < boot_params.e820_entries; ++i) {
        int type = boot_params.e820_table[i].type - 1;
        if (type < 0 || type > 3) type = 1;
        printf("%08x%08x %08x%08x %s\n",
            (u32)(boot_params.e820_table[i].addr >> 32),
            (u32)boot_params.e820_table[i].addr,
            (u32)(boot_params.e820_table[i].size >> 32),
            (u32)boot_params.e820_table[i].size,
            addr_range_type[type]);
    }
}

extern "C" int main()
{
    /* First, copy the boot header into the "zeropage" */
	copy_boot_params();
	// debug_print_boot_params();

	/* End of heap check */
	init_heap();

	/* Tell the BIOS what CPU mode we intend to run in. */
	set_bios_mode();

    /* Detect memory layout */
    detect_memory();
	// debug_print_memory_layout();

	/* Set keyboard repeat rate (why?) and query the lock flags */
	keyboard_init();

	/* Set the video mode */
	set_video();

	/* Do the last things and invoke protected mode */
	go_to_protected_mode();
}
