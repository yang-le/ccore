#pragma once

/* Undef any of these macros coming from string_32.h. */
#undef memcpy
#undef memset
#undef memcmp

extern "C" void *memcpy(void *dst, const void *src, size_t len);
extern "C" void *memset(void *dst, int c, size_t len);
extern "C" int memcmp(const void *s1, const void *s2, size_t len);

/* Access builtin version by default. */
#define memcpy(d,s,l) __builtin_memcpy(d,s,l)
#define memset(d,c,l) __builtin_memset(d,c,l)
#define memcmp	__builtin_memcmp

extern size_t strnlen(const char *s, size_t maxlen);
