#include <graphx.h>
#include <string.h>
#undef NDEBUG
#define DEBUG
#include <debug.h>
#include "patch.h"
#include "flash.h"

bool patch(void *location, const void *replacement, const void *old, size_t size) {
    boot_code_to_vram();
    if(memcmp((uint24_t)location + gfx_vram, old, size) != 0) {
        dbg_sprintf(dbgout, "data @ %p does not match old\n", location);
        (*(volatile unsigned char *) 0xFFFFFF = 2);
        return false;
    }
    memcpy((uint24_t)location + gfx_vram, replacement, size);
    vram_to_boot_code();

    return true;
}
