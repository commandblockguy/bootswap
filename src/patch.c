#include <graphx.h>
#include <string.h>
#undef NDEBUG
#define DEBUG
#include <debug.h>
#include "patch.h"
#include "flash.h"

bool patch(void *location, const void *replacement, const void *old, size_t size) {
    boot_code_to_vram();
    if(memcmp((uint24_t)location + gfx_vram, replacement, size) == 0) return true;
    if(memcmp((uint24_t)location + gfx_vram, old, size) != 0) {
        dbg_sprintf(dbgout, "data @ %p does not match old\n", location);
        return false;
    }
    memcpy((uint24_t)location + gfx_vram, replacement, size);
    if(!vram_to_boot_code())
        if(!vram_to_boot_code())
            if(!vram_to_boot_code())
                return false;

    return true;
}
