#pragma once

#include <linux/types.h>

enum {
	GATE_INTERRUPT = 0xE,
	GATE_TRAP = 0xF,
	GATE_CALL = 0xC,
	GATE_TASK = 0x5,
};

struct [[gnu::packed]] idt_bits {
	u16		ist	    : 3,
			zero	: 5,
			type	: 5,
			dpl	    : 2,
			p	    : 1;
};

struct [[gnu::packed]] gate_desc {
	u16		offset_low;
	u16		segment;
	struct idt_bits	bits;
	u16		offset_middle;
	u32		offset_high;
	u32		reserved;
};

struct [[gnu::packed]] desc_ptr {
	unsigned short size;
	unsigned long address;
};
