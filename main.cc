#include "boot.h"

alignas(16)
struct boot_params boot_params;

extern "C" int main()
{
    puts("hello world!\n");

    detect_memory();

    const char* addr_range_type[] = {
        "Available",
        "Reserved",
        "ACPI Reclaim",
        "ACPI NVS"
    };

    printf("\nMemory Info:\n");
    printf("\n%-16s %-16s %s\n", "addr", "size", "type");
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

	/* Do the last things and invoke protected mode */
	go_to_protected_mode();
}
