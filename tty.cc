#include "boot.h"

[[gnu::section(".inittext")]]
static void bios_putchar(int ch)
{
	biosregs ireg;

	initregs(ireg);
	ireg.bx = 0x0007;
	ireg.cx = 0x0001;
	ireg.ah = 0x0e;
	ireg.al = ch;
	intcall(0x10, &ireg, nullptr);
}

[[gnu::section(".inittext")]]
void putchar(int ch)
{
	if (ch == '\n')
		bios_putchar('\r');	/* \n -> \r\n */

	bios_putchar(ch);
}

[[gnu::section(".inittext")]]
extern "C" void puts(const char *str)
{
	while (*str)
		putchar(*str++);
}
