#pragma once

#include "types.h"

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

struct [[gnu::packed]] boot_params {
    u8 e820_entries;
    u8 __pad[3];
    boot_e820_entry e820_table[E820_MAX_ENTRIES_ZEROPAGE];
};
