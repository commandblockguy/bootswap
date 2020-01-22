#include <fileioc.h>
#undef NDEBUG
#define DEBUG
#include <debug.h>
#include <graphx.h>
#include "ui.h"
#include "verification.h"
#include "flash.h"
#include "asm/flash.h"

#define debugger(ignore) (*(volatile unsigned char *) 0xFFFFFF = 2);

void main() {

    ti_CloseAll();

    gfx_Begin();
    gfx_SetTextTransparentColor(42);
    gfx_SetTextBGColor(42);
    gfx_SetDrawBuffer();

    if(uses_new_flash()) {
        message("Incompatible", "BootSwap is not compatible with calculators with hardware revision M or higher, which includes your calculator.");
    } else {
        message("Warning", "If used improperly, this program could cause your calculator to become permanently inoperable. Please read the entire README prior to using this program. I am not responsible for any damage caused by improper use of this program.");

        main_menu();
    }

    gfx_End();
    ti_CloseAll();
    ti_Delete("TMP");
}
