#pragma once

/* setup_data/setup_indirect types */
#define SETUP_NONE			0
#define SETUP_E820_EXT			1
#define SETUP_DTB			2
#define SETUP_PCI			3
#define SETUP_EFI			4
#define SETUP_APPLE_PROPERTIES		5
#define SETUP_JAILHOUSE			6

#define SETUP_INDIRECT			(1<<31)

/* SETUP_INDIRECT | max(SETUP_*) */
#define SETUP_TYPE_MAX			(SETUP_INDIRECT | SETUP_JAILHOUSE)

/* ram_size flags */
#define RAMDISK_IMAGE_START_MASK	0x07FF
#define RAMDISK_PROMPT_FLAG		0x8000
#define RAMDISK_LOAD_FLAG		0x4000

/* loadflags */
#define LOADED_HIGH	(1<<0)
#define KASLR_FLAG	(1<<1)
#define QUIET_FLAG	(1<<5)
#define KEEP_SEGMENTS	(1<<6)
#define CAN_USE_HEAP	(1<<7)

/* xloadflags */
#define XLF_KERNEL_64			(1<<0)
#define XLF_CAN_BE_LOADED_ABOVE_4G	(1<<1)
#define XLF_EFI_HANDOVER_32		(1<<2)
#define XLF_EFI_HANDOVER_64		(1<<3)
#define XLF_EFI_KEXEC			(1<<4)
#define XLF_5LEVEL			(1<<5)
#define XLF_5LEVEL_ENABLED		(1<<6)

#ifndef __ASSEMBLY__

#include <linux/types.h>
#include <linux/screen_info.h>

struct [[gnu::packed]] setup_header {
	u8	setup_sects;
	u16	root_flags;
	u32	syssize;
	u16	ram_size;
	u16	vid_mode;
	u16	root_dev;
	u16	boot_flag;
	u16	jump;
	u32	header;
	u16	version;
	u32	realmode_swtch;
	u16	start_sys_seg;
	u16	kernel_version;
	u8	type_of_loader;
	u8	loadflags;
	u16	setup_move_size;
	u32	code32_start;
	u32	ramdisk_image;
	u32	ramdisk_size;
	u32	bootsect_kludge;
	u16	heap_end_ptr;
	u8	ext_loader_ver;
	u8	ext_loader_type;
	u32	cmd_line_ptr;
	u32	initrd_addr_max;
	u32	kernel_alignment;
	u8	relocatable_kernel;
	u8	min_alignment;
	u16	xloadflags;
	u32	cmdline_size;
	u32	hardware_subarch;
	u64	hardware_subarch_data;
	u32	payload_offset;
	u32	payload_length;
	u64	setup_data;
	u64	pref_address;
	u32	init_size;
	u32	handover_offset;
	u32	kernel_info_offset;
};

#define EDD_MBR_SIG_MAX 16        /* max number of signatures to store */

/*
 * This is the maximum number of entries in struct boot_params::e820_table
 * (the zeropage), which is part of the x86 boot protocol ABI:
 */
#define E820_MAX_ENTRIES_ZEROPAGE 128

/*
 * The E820 memory region entry of the boot protocol ABI:
 */
struct [[gnu::packed]] boot_e820_entry {
	u64 addr;
	u64 size;
	u32 type;
};

/* The so-called "zeropage" */
struct [[gnu::packed]] boot_params {
    screen_info screen_info;		/* 0x000 */
	u8 apm_bios_info[0x14];		    /* 0x040 */
	u8  _pad2[4];					/* 0x054 */
	u64  tboot_addr;				/* 0x058 */
	u8 ist_info[0x10];			    /* 0x060 */
	u64 acpi_rsdp_addr;				/* 0x070 */
	u8  _pad3[8];					/* 0x078 */
	u8  hd0_info[16];	/* obsolete! */		/* 0x080 */
	u8  hd1_info[16];	/* obsolete! */		/* 0x090 */
	u8 sys_desc_table[0x10]; /* obsolete! */	/* 0x0a0 */
	u8 olpc_ofw_header[0x10];		/* 0x0b0 */
	u32 ext_ramdisk_image;			/* 0x0c0 */
	u32 ext_ramdisk_size;				/* 0x0c4 */
	u32 ext_cmd_line_ptr;				/* 0x0c8 */
	u8  _pad4[116];				/* 0x0cc */
	u8 edid_info[0x80];			/* 0x140 */
	u8 efi_info[0x20];			/* 0x1c0 */
	u32 alt_mem_k;				/* 0x1e0 */
	u32 scratch;		/* Scratch field! */	/* 0x1e4 */
	u8  e820_entries;				/* 0x1e8 */
	u8  eddbuf_entries;				/* 0x1e9 */
	u8  edd_mbr_sig_buf_entries;			/* 0x1ea */
	u8  kbd_status;				/* 0x1eb */
	u8  secure_boot;				/* 0x1ec */
	u8  _pad5[2];					/* 0x1ed */
	/*
	 * The sentinel is set to a nonzero value (0xff) in header.S.
	 *
	 * A bootloader is supposed to only take setup_header and put
	 * it into a clean boot_params buffer. If it turns out that
	 * it is clumsy or too generous with the buffer, it most
	 * probably will pick up the sentinel variable too. The fact
	 * that this variable then is still 0xff will let kernel
	 * know that some variables in boot_params are invalid and
	 * kernel should zero out certain portions of boot_params.
	 */
	u8  sentinel;					/* 0x1ef */
	u8  _pad6[1];					/* 0x1f0 */
	setup_header hdr;    /* setup header */	/* 0x1f1 */
	u8  _pad7[0x290 - 0x1f1 - sizeof(setup_header)];
	u32 edd_mbr_sig_buffer[EDD_MBR_SIG_MAX];	/* 0x290 */
	boot_e820_entry e820_table[E820_MAX_ENTRIES_ZEROPAGE]; /* 0x2d0 */
	u8  _pad8[48];				/* 0xcd0 */
	u8 eddbuf[0x1ec];		    /* 0xd00 */
	u8  _pad9[276];				/* 0xeec */
};

#endif /* __ASSEMBLY__ */
