#pragma once

#define __FORCE_ORDER "m"(*(unsigned int *)0x1000UL)

static inline unsigned long native_read_cr2(void)
{
	unsigned long val;
	asm volatile("mov %%cr2,%0\n\t" : "=r" (val) : __FORCE_ORDER);
	return val;
}
