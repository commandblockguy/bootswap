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
    void *sequence = NULL, *mod_loc = NULL;
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

    os_ClrHomeFull();
    os_SetCursorPos(0, 0);

    debugger();
    unlock_bootcode();

    timer_Control = TIMER1_DISABLE;
    timer_1_Counter = 0;
    timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_UP;

    timer_Control = TIMER1_DISABLE;

    /* Disable OS verification */
    //write_bytes(mod_loc, overwrite_data, sizeof(overwrite_data));

    /* Add null terminator to "RAM cleared" in the OS */
    //write_bytes((void*)0x08D6FC, &zero, 1);

    lock_bootcode();

    *(uint24_t*)gfx_vram = 0xffffff;

    while(!os_GetCSC());

    exit:

    ti_CloseAll();
    ti_Delete("TMP");
}
