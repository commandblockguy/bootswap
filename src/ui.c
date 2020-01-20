#include <stdint.h>
#include <stdbool.h>
#include <keypadc.h>
#include <tice.h>
#include <graphx.h>
#undef NDEBUG
#define DEBUG
#include <debug.h>
#include "ui.h"
#include "backup.h"
#include "flash.h"

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

#define BG_COLOR 0
#define TEXT_COLOR 255
#define SELECTED_COLOR 60

void menu_redraw(const char *name, const struct menu_option *options, uint8_t num_options, uint8_t selected) {
    const uint24_t base_x = LCD_WIDTH / 8;
    const uint8_t title_y = 8;
    const uint8_t base_y = 32;
    const uint8_t option_distance = 12;
    uint8_t i;

    gfx_FillScreen(BG_COLOR);

    gfx_SetTextFGColor(TEXT_COLOR);
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY(name, base_x, title_y);
    gfx_SetTextScale(1, 1);


    for(i = 0; i < num_options; i++) {
        if(i == selected) {
            gfx_SetTextFGColor(SELECTED_COLOR);
        } else {
            gfx_SetTextFGColor(TEXT_COLOR);
        }

        gfx_PrintStringXY(options[i].str, base_x, base_y + i * option_distance);
    }

    gfx_SwapDraw();
}

void menu(const char *name, const struct menu_option *options, uint8_t num_options) {
    uint8_t selected = 0;

    menu_redraw(name, options, num_options, selected);

    while(true) {
        kb_Scan();

        if(kb_IsDown(kb_KeyClear)) break;

        if(kb_IsDown(kb_KeyUp)) {
            if(selected == 0) selected = num_options;
            selected--;
            menu_redraw(name, options, num_options, selected);

            delay(100);
        }

        if(kb_IsDown(kb_KeyDown)) {
            selected++;
            if(selected == num_options) selected = 0;
            menu_redraw(name, options, num_options, selected);

            delay(100);
        }

        if(kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd)) {
            if(options[selected].function == NULL) return;
            while(kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd)) kb_Scan();

            options[selected].function();

            menu_redraw(name, options, num_options, selected);
        }
    }
}

void main_menu(void) {
    const struct menu_option options[] = {
        {menu_backup, "Back up to appvar"},
        {menu_install, "Install from appvar"},
        {menu_verify_current, "Verify current bootcode"},
        {menu_verify_appvar, "Verify appvar"},
        {menu_enable_3P, "Enable 3rd-party OSes"},
        {menu_disable_3P, "Disable 3rd-party OSes"},
        {NULL, "Exit"}
    };
    const char num_options = sizeof(options) / sizeof(options[0]);

    menu("BootSwap", options, num_options);
}

void menu_backup(void) {
    /* todo: ask for appvar name */
    boot_code_to_vram();
    vram_to_appvar("BOOT");
}

void menu_install(void) {
    /* todo: list appvars */
    appvar_to_vram("BOOT");
    vram_to_boot_code();
}

void menu_verify_current(void) {
    //todo: verify current bootcode
}

void menu_verify_appvar(void) {
    //todo: verify appvar
}

void menu_enable_3P(void) {
    //todo: patch bootcode
}

void menu_disable_3P(void) {
    //todo: patch bootcode
}

