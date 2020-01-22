#include <fileioc.h>
#undef NDEBUG
#define DEBUG
#include <debug.h>
#include <graphx.h>
#include "ui.h"
#include "verification.h"
#include "flash.h"

#define debugger(ignore) (*(volatile unsigned char *) 0xFFFFFF = 2);

void main() {

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
