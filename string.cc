#include <linux/types.h>
#include "string.h"

/*
 * Undef these macros so that the functions that we provide
 * here will have the correct names regardless of how string.h
 * may have chosen to #define them.
 */
#undef memcpy
#undef memset
#undef memcmp

size_t strnlen(const char *s, size_t maxlen)
{
	const char *es = s;
	while (*es && maxlen) {
		es++;
		maxlen--;
	}

	return (es - s);
}
