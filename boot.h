#pragma once

#include "types.h"
#include "stdarg.h"
#include "ctype.h"
#include "string.h"
#include "bootparam.h"

/* Useful macros */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

extern struct boot_params boot_params;

/* Basic port I/O */
static inline void outb(u8 v, u16 port)
{
	/* a -> ax d -> dx N -> uint for in/out instruction */
	asm volatile("outb %[v], %[port]" : : [v] "a" (v), [port] "dN" (port));
}

static inline u8 inb(u16 port)
{
	u8 v;
	/* = -> write only */
	asm volatile("inb %[port], %[v]" : [v] "=a" (v) : [port] "dN" (port));
	return v;
}

static inline void outw(u16 v, u16 port)
{
	asm volatile("outw %[v], %[port]" : : [v] "a" (v), [port] "dN" (port));
}

static inline u16 inw(u16 port)
{
	u16 v;
	asm volatile("inw %[port], %[v]" : [v] "=a" (v) : [port] "dN" (port));
	return v;
}

static inline void outl(u32 v, u16 port)
{
	asm volatile("outl %[v], %[port]" : : [v] "a" (v), [port] "dN" (port));
}

static inline u32 inl(u16 port)
{
	u32 v;
	asm volatile("inl %[port], %[v]" : [v] "=a" (v) : [port] "dN" (port));
	return v;
}

static inline void io_delay(void)
{
	const u16 DELAY_PORT = 0x80;
	asm volatile("outb %%al, %[port]" : : [port] "dN" (DELAY_PORT));
}

/* These functions are used to reference data in other segments. */

static inline u16 ds(void)
{
	u16 seg;
	/* r -> reg m -> mem */
	asm volatile("movw %%ds, %[seg]" : [seg] "=rm" (seg));
	return seg;
}

static inline u16 fs(void)
{
	u16 seg;
	asm volatile("movw %%fs, %[seg]" : [seg] "=rm" (seg));
	return seg;
}

static inline u16 gs(void)
{
	u16 seg;
	asm volatile("movw %%gs, %[seg]" : [seg] "=rm" (seg));
	return seg;
}

static inline void set_fs(u16 seg)
{
	asm volatile("movw %[seg], %%fs" : : [seg] "rm" (seg));
}

static inline void set_gs(u16 seg)
{
	asm volatile("movw %[seg], %%gs" : : [seg] "rm" (seg));
}

typedef unsigned int addr_t;

static inline u8 rdfs8(addr_t addr)
{
	u8 v;
	/* q -> lower half reg */
	asm volatile("movb %%fs:%[addr], %[v]" : [v] "=q" (v) : [addr] "m" (*(u8 *)addr));
	return v;
}

static inline u16 rdfs16(addr_t addr)
{
	u16 v;
	asm volatile("movw %%fs:%[addr], %[v]" : [v] "=r" (v) : [addr] "m" (*(u16 *)addr));
	return v;
}

static inline u32 rdfs32(addr_t addr)
{
	u32 v;
	asm volatile("movl %%fs:%[addr], %[v]" : [v] "=r" (v) : [addr] "m" (*(u32 *)addr));
	return v;
}

static inline void wrfs8(u8 v, addr_t addr)
{
	/* + -> read/write i -> imm */
	asm volatile("movb %[v], %%fs:%[addr]" : [addr] "+m" (*(u8 *)addr) : [v] "qi" (v));
}

static inline void wrfs16(u16 v, addr_t addr)
{
	asm volatile("movw %[v], %%fs:%[addr]" : [addr] "+m" (*(u16 *)addr) : [v] "ri" (v));
}

static inline void wrfs32(u32 v, addr_t addr)
{
	asm volatile("movl %[v], %%fs:%[addr]" : [addr] "+m" (*(u32 *)addr) : [v] "ri" (v));
}

static inline u8 rdgs8(addr_t addr)
{
	u8 v;
	asm volatile("movb %%gs:%[addr], %[v]" : [v] "=q" (v) : [addr] "m" (*(u8 *)addr));
	return v;
}

static inline u16 rdgs16(addr_t addr)
{
	u16 v;
	asm volatile("movw %%gs:%[addr], %[v]" : [v] "=r" (v) : [addr] "m" (*(u16 *)addr));
	return v;
}

static inline u32 rdgs32(addr_t addr)
{
	u32 v;
	asm volatile("movl %%gs:%[addr], %[v]" : [v] "=r" (v) : [addr] "m" (*(u32 *)addr));
	return v;
}

static inline void wrgs8(u8 v, addr_t addr)
{
	asm volatile("movb %[v], %%gs:%[addr]" : [addr] "+m" (*(u8 *)addr) : [v] "qi" (v));
}

static inline void wrgs16(u16 v, addr_t addr)
{
	asm volatile("movw %[v], %%gs:%[addr]" : [addr] "+m" (*(u16 *)addr) : [v] "ri" (v));
}

static inline void wrgs32(u32 v, addr_t addr)
{
	asm volatile("movl %[v], %%gs:%[addr]" : [addr] "+m" (*(u32 *)addr) : [v] "ri" (v));
}

/* bioscall.c */
struct biosregs {
	union {
		struct {
			u32 edi;
			u32 esi;
			u32 ebp;
			u32 _esp;
			u32 ebx;
			u32 edx;
			u32 ecx;
			u32 eax;
			u32 _fsgs;
			u32 _dses;
			u32 eflags;
		};
		struct {
			u16 di, hdi;
			u16 si, hsi;
			u16 bp, hbp;
			u16 _sp, _hsp;
			u16 bx, hbx;
			u16 dx, hdx;
			u16 cx, hcx;
			u16 ax, hax;
			u16 gs, fs;
			u16 es, ds;
			u16 flags, hflags;
		};
		struct {
			u8 dil, dih, edi2, edi3;
			u8 sil, sih, esi2, esi3;
			u8 bpl, bph, ebp2, ebp3;
			u8 _spl, _sph, _esp2, _esp3;
			u8 bl, bh, ebx2, ebx3;
			u8 dl, dh, edx2, edx3;
			u8 cl, ch, ecx2, ecx3;
			u8 al, ah, eax2, eax3;
		};
	};
};

extern "C" void intcall(u8 int_no, const struct biosregs *ireg, struct biosregs *oreg);

/* regs.cc */
void initregs(biosregs& regs);

/* tty.cc */
void puts(const char *);
void putchar(int);

/* memory.c */
void detect_memory(void);

/* printf.c */
int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);
int printf(const char *fmt, ...);
