#include <fileioc.h>
#include <string.h>
#include <tice.h>
#include <stddef.h>
#undef NDEBUG
#define DEBUG
#include <debug.h>
#include "versions.h"

extern void* seq;
extern void* mod_loc;

void asmprgm();

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

void *getModLoc(void) {
    uint8_t i;
    const system_info_t *sys_info = os_GetSystemInfo();
    for(i = 0; i < NUM_VERSIONS; i++) {
        const struct version_number *current = &versions[i].number;
        dbg_sprintf(dbgout, "%p,%p\n", current, &sys_info->bootMajorVersion);
        if(memcmp(current, &sys_info->bootMajorVersion, sizeof(struct version_number)) == 0) {
            return versions[i].nop_location;
        }
    }
    os_PutStrFull("incompatible");
    return NULL;
}

void main() {
    dbg_Debugger();

    // todo:
    /* Display warning */
    /* Check which flash chip is in use */
    /* Select options: */
    /*  Dump boot code to appvar */
    /*   Ask for appvar name */
    /*   Use the privileged ldir to copy to vRAM */
    /*  Load boot code from appvar */
    /*   Another warning */
    /*   Copy data from appvar to vRAM */
    /*   Basic sanity checks */
    /*    Is the boot code to be installed in our version list? */
    /*    Is this a boot code version that's in use on post-rev-M calcs? */
    /*    Does this have proper interrupt handlers? */
    /*    Other locational bits needed for regular calculator operation */
    /*   Unlock flash as before */
    /*   Wipe and flash one sector at a time */
    /*   Relock flash */
    /*  Check if stock bootcode */
    /*   Take some sort of checksum of the bootcode */
    /*   Display which bootcode version it matched */
    /*  Allow 3rd-party OSes */
    /*   Another warning */
    /*   Basic sanity check to ensure that the location is correct */
    /*   Copy relevant sector to vRAM */
    /*   Make changes to the vRAM */
    /*   Erase and flash sector like before */
    /*  Revert 3rd-party OS mod */
    /*   Same as allowing in the first place */

    /* Maybe an "apply diff" mode? */

    seq = getSequence();
    mod_loc = getModLoc();
    if(mod_loc) {
        asmprgm();
    }

    /* todo: Delete the appvar with the sequence */
}
