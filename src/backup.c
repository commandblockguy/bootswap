#include <fileioc.h>
#include <graphx.h>
#include <string.h>
#include <debug.h>
#include "backup.h"

#define CUTOFF_LENGTH 65000

bool vram_to_appvar(char *name) {
    ti_var_t slot;
    const uint24_t var_1_size = 0x020000 - 2 * CUTOFF_LENGTH;
    char name_buf[9];
    uint8_t name_length = strlen(name);

    strcpy(name_buf, name);

    slot = ti_Open(name_buf, "w");
    if(!slot) return false;
    ti_Write("bootcode", 1, 9, slot);
    ti_Write(gfx_vram, 1, var_1_size, slot);
    ti_SetArchiveStatus(true, slot);
    ti_Close(slot);

    name_buf[name_length] = '1';
    name_buf[name_length + 1] = 0;

    dbg_sprintf(dbgout, "name: %s", name_buf);

    slot = ti_Open(name_buf, "w");
    if(!slot) return false;
    ti_Write(&gfx_vram[var_1_size], 1, CUTOFF_LENGTH, slot);
    ti_SetArchiveStatus(true, slot);
    ti_Close(slot);

    name_buf[name_length] = '2';

    slot = ti_Open(name_buf, "w");
    if(!slot) return false;
    ti_Write(&gfx_vram[var_1_size + CUTOFF_LENGTH], 1, CUTOFF_LENGTH, slot);
    ti_SetArchiveStatus(true, slot);
    ti_Close(slot);

    return true;
}

bool appvar_to_vram(char* name) {
    ti_var_t slot;
    const uint24_t var_1_size = 0x020000 - 2 * CUTOFF_LENGTH;
    char name_buf[9];
    char test_buf[9];
    uint8_t name_length = strlen(name);

    strcpy(name_buf, name);

    slot = ti_Open(name_buf, "r");
    if(!slot) return false;
    ti_Read(test_buf, 1, 9, slot);
    if(strcmp(test_buf, "bootcode") != 0) return false;

    ti_Read(gfx_vram, 1, var_1_size, slot);
    ti_Close(slot);

    name_buf[name_length] = '1';
    name_buf[name_length + 1] = 0;

    dbg_sprintf(dbgout, "name: %s", name_buf);

    slot = ti_Open(name_buf, "r");
    if(!slot) return false;
    ti_Read(&gfx_vram[var_1_size], 1, CUTOFF_LENGTH, slot);
    ti_Close(slot);

    name_buf[name_length] = '2';

    slot = ti_Open(name_buf, "r");
    if(!slot) return false;
    ti_Read(&gfx_vram[var_1_size + CUTOFF_LENGTH], 1, CUTOFF_LENGTH, slot);
    ti_Close(slot);

    return true;
}