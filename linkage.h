#pragma once

/* SYM_A_* -- align the symbol? */
#define SYM_A_ALIGN				.p2align 4, 0x90
#define SYM_A_NONE				/* nothing */

/* SYM_L_* -- linkage of symbols */
#define SYM_L_GLOBAL(name)			.globl name
#define SYM_L_WEAK(name)			.weak name
#define SYM_L_LOCAL(name)			/* nothing */

#define SYM_ENTRY(name, linkage, align...)  \
    linkage(name);                          \
    align ;                                 \
    name:

#define SYM_START(name, linkage, align...) \
    SYM_ENTRY(name, linkage, align)

#define SYM_END(name, sym_type)             \
    .type name sym_type ;                   \
    .set .L__sym_size_##name, . - name ;    \
    .size name, .L__sym_size_##name

#define SYM_FUNC_END(name)                  \
    SYM_END(name, @function)

/* SYM_FUNC_START -- use for global functions */
#define SYM_FUNC_START(name)				\
	SYM_START(name, SYM_L_GLOBAL, SYM_A_ALIGN)

/* SYM_FUNC_START_NOALIGN -- use for global functions, w/o alignment */
#define SYM_FUNC_START_NOALIGN(name)			\
	SYM_START(name, SYM_L_GLOBAL, SYM_A_NONE)

/* SYM_FUNC_START_LOCAL -- use for local functions */
#define SYM_FUNC_START_LOCAL(name)			\
	SYM_START(name, SYM_L_LOCAL, SYM_A_ALIGN)

/* SYM_FUNC_START_LOCAL_NOALIGN -- use for local functions, w/o alignment */
#define SYM_FUNC_START_LOCAL_NOALIGN(name)		\
	SYM_START(name, SYM_L_LOCAL, SYM_A_NONE)

/* SYM_FUNC_START_WEAK -- use for weak functions */
#define SYM_FUNC_START_WEAK(name)			\
	SYM_START(name, SYM_L_WEAK, SYM_A_ALIGN)

/* SYM_FUNC_START_WEAK_NOALIGN -- use for weak functions, w/o alignment */
#define SYM_FUNC_START_WEAK_NOALIGN(name)		\
	SYM_START(name, SYM_L_WEAK, SYM_A_NONE)
