#pragma once

/* Internal svga startup constants */
#define NORMAL_VGA	0xffff		/* 80x25 mode */
#define EXTENDED_VGA	0xfffe		/* 80x50 mode */
#define ASK_VGA		0xfffd		/* ask for it at bootup */

#define BOOT_STACK_SIZE	    0x4000
#define BOOT_HEAP_SIZE		0x10000

#define BOOT_INIT_PGT_SIZE	(6*4096)

/* Boot IDT definitions */
#define	BOOT_IDT_ENTRIES	32
