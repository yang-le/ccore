#pragma once

#include <linux/linkage.h>
#include <asm/boot.h>
#include <asm/bootparam.h>
#include <asm/io.h>

#include "../ctype.h"

/* misc.c */
extern struct boot_params *boot_params;
void __putstr(const char *s);
void __puthex(unsigned long value);
#define error_putstr(__x)  __putstr(__x)
#define error_puthex(__x)  __puthex(__x)
