#include <fileioc.h>
#include <string.h>
#include <tice.h>
#include <stddef.h>
#undef NDEBUG
#define DEBUG
#include <debug.h>
#include <graphx.h>
#include "versions.h"
#include "ui.h"

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

    ti_CloseAll();

    gfx_Begin();
    gfx_SetTextTransparentColor(42);
    gfx_SetTextBGColor(42);
    gfx_SetDrawBuffer();

    main_menu();

    exit:

    gfx_End();
    ti_CloseAll();
    ti_Delete("TMP");
}
