#include "error.h"
#include "misc.h"

/* Locates and clears a region for a new top level page table. */
extern "C" void initialize_identity_maps(void *rmode)
{
    __putstr("initialize_identity_maps\n");
}
