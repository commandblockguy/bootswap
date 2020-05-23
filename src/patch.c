#include <graphx.h>
#include <string.h>
#undef NDEBUG
#define DEBUG
#include <debug.h>
#include "patch.h"
#include "flash.h"

bool apply_patch(void *location, const void *replacement, const void *old, size_t size) {
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

// 0x647B 38 06

/* ld hl,1 \ ret */
const uint8_t   signature_patch_data[] = {0x21, 0x01, 0x00, 0x00, 0xC9};
const uint8_t signature_unpatch_data[] = {0xDD, 0xE5, 0xDD, 0x21, 0x00};

const uint8_t jr_nop_data[] = {0x00, 0x00};

const uint8_t downgrade_unpatch_1_data[] = {0x38, 0x0C};
const uint8_t downgrade_unpatch_2_data[] = {0x38, 0x06};

const uint8_t model_unpatch_data[] = {0x20, 0x06};

const struct patch patches[NUM_PATCHES] = {
    {5, signature_patch_data,   signature_unpatch_data},
    {2,          jr_nop_data, downgrade_unpatch_1_data},
    {2,          jr_nop_data, downgrade_unpatch_2_data},
    {2,          jr_nop_data,       model_unpatch_data}
};
