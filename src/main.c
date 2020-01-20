#include <fileioc.h>
#include <string.h>
#include <tice.h>
#include <stddef.h>
#undef NDEBUG
#define DEBUG
#include <debug.h>
#include <graphx.h>
#include "versions.h"
#include "asm/ports.h"
#include "asm/flash.h"
#include "flash.h"
#include "backup.h"

#define debugger(ignore) (*(volatile unsigned char *) 0xFFFFFF = 2);

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
    void *mod_loc = NULL;
    // actual data there is DDE5DD21000000DD...
    //                      00210100000000C9
    // ld hl is 0x21, ret is 0xC9
    const char overwrite_data[] = {00,0x21,01,00,00,00,00,0xC9};
    const char zero = 0;


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

    dbg_sprintf(dbgout, "\n\nprogram started\n");

    ti_CloseAll();

    boot_code_to_vram();

    dbg_sprintf(dbgout, "boot code copied to vram\n");

    if(!vram_to_appvar("BOOT")) {
        dbg_sprintf(dbgout, "error loading to appvar\n");
        goto exit;
    }

    while(!os_GetCSC());

    os_ClrHomeFull();

    if(!appvar_to_vram("BOOT")) {
        dbg_sprintf(dbgout, "error in loading from appvar\n");
        goto exit;
    }

    vram_to_boot_code();

    while(!os_GetCSC());

    exit:

    ti_CloseAll();
    ti_Delete("TMP");
}
