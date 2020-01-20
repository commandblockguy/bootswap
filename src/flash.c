#include <fileioc.h>
#include "flash.h"
#include "asm/flash.h"
#include "asm/ports.h"
#undef NDEBUG
#define DEBUG
#include <debug.h>
#include <graphx.h>

void *sequence = NULL;

/* Privileged code must be reset prior to running this */
void *getSequence(void) {
    static const uint8_t seq[] = {0x3E, 0x04, 0xF3, 0x18, 0x00, 0xF3, 0xED, 0x7E, 0xED, 0x56, 0xED, 0x39, 0x28, 0xED, 0x38, 0x28, 0xCB, 0x57, 0xC9};
    void *ptr;
    ti_var_t slot;
    ti_CloseAll();
    /* todo: Error if file exists? */
    slot = ti_Open("TMP", "w");
    ti_Write(seq, sizeof(seq), 1, slot);
    ti_SetArchiveStatus(slot, true);
    ti_Rewind(slot);
    ptr = ti_GetDataPtr(slot);
    return ptr;
}

bool write_sector(void *sector, const void *data) {
    erase_sector(sector);
    if(sector < (void*)65536) {
        write_bytes(sector, data, 8192);
    } else {
        write_bytes(sector, data, 65535);
    }
}

void unlock_bootcode(void) {
    if(!sequence) {
        if(priv_upper() == 0xFF) {
            reset_priv();
        }
        sequence = getSequence();
    }
    set_priv();
    if(get_flash_lock_status() == LOCKED)
        flash_unlock();
    if(get_flash_lock_status() == PARTIALLY_UNLOCKED)
        flash_sequence(sequence);
    reset_all_ipbs();
}

void lock_bootcode(void) {
    /* We can't lock ipbs if flash itself is locked */
    if(get_flash_lock_status() != FULLY_UNLOCKED) {
        dbg_sprintf(dbgout, "flash is not fully unlocked\n");
        unlock_bootcode();
    }
    set_boot_ipbs();
    flash_lock();
    reset_priv();
}

void boot_code_to_vram(void) {
    priv_copy(gfx_vram, 0, 0x020000);
}

void vram_to_boot_code(void) {
    void *sector;

    unlock_bootcode();

    for(sector = 0x000000; sector <= (void*)0x010000; sector += 0x2000) {
        write_sector(sector, gfx_vram + (int)sector);
    }

    lock_bootcode();
}
