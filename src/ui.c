#include <stdint.h>
#include <stdbool.h>
#include <keypadc.h>
#include <tice.h>
#include <graphx.h>
#undef NDEBUG
#define DEBUG
#include <debug.h>
#include <fileioc.h>
#include <string.h>
#include "ui.h"
#include "backup.h"
#include "flash.h"
#include "patch.h"
#include "versions.h"

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

#define BASE_X LCD_WIDTH / 8
#define TITLE_Y 8
#define BASE_Y 32
#define LINE_SPACING 12

void menu_redraw(const char *name, const struct menu_option *options, uint8_t num_options, uint8_t selected) {
    uint8_t i;

    gfx_FillScreen(BG_COLOR);

    gfx_SetTextFGColor(TEXT_COLOR);
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY(name, BASE_X, TITLE_Y);
    gfx_SetTextScale(1, 1);


    for(i = 0; i < num_options; i++) {
        if(i == selected) {
            gfx_SetTextFGColor(SELECTED_COLOR);
        } else {
            gfx_SetTextFGColor(TEXT_COLOR);
        }

        gfx_PrintStringXY(options[i].str, BASE_X, BASE_Y + i * LINE_SPACING);
    }

    gfx_SwapDraw();
}

/* Returns 0 if clear was pressed, or the option number starting at 1 */
uint8_t menu(const char *name, const struct menu_option *options, uint8_t num_options) {
    uint8_t selected = 0;

    menu_redraw(name, options, num_options, selected);

    while(true) {
        kb_Scan();

        if(kb_IsDown(kb_KeyClear)) return 0;

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
            if(options[selected].function == NULL) return selected + 1;
            while(kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd)) kb_Scan();

            options[selected].function();

            menu_redraw(name, options, num_options, selected);
            return selected + 1;
        }
    }
}

void main_menu(void) {
    const struct menu_option options[] = {
        {menu_backup,               "Back up to appvar"},
        {menu_install,              "Install from appvar"},
        {menu_verify_current,       "Verify current bootcode"},
        {menu_verify_appvar,        "Verify appvar"},
        {menu_disable_verification, "Disable OS verification"},
        {menu_enable_verification,  "Re-enable OS verification"},
        {NULL,                      "Exit"}
    };
    const char num_options = sizeof(options) / sizeof(options[0]);

    uint8_t val;

    while(true) {
        val = menu("BootSwap", options, num_options);
        if(val == 0) break;
        if(val == num_options) break;
    }
}

void menu_backup(void) {
    /* todo: ask for appvar name */
    boot_code_to_vram();
    vram_to_appvar("BOOT");
}

void menu_install(void) {
    struct menu_option options[10];
    char names[10][9];
    void *search_pos;
    uint8_t num_vars, selection;

    for(search_pos = NULL, num_vars = 0; num_vars < 100; num_vars++) {
        char *name;
        options[num_vars].function = NULL;
        options[num_vars].str = names[num_vars];
        if(!(name = ti_Detect(&search_pos, "bootcode"))) break;
        strcpy(names[num_vars], name);
        // todo: maybe ignore appvars with missing data
    }

    if(!num_vars) {
        gfx_FillScreen(BG_COLOR);
        gfx_SetTextFGColor(TEXT_COLOR);
        gfx_PrintStringXY("No backups found.", BASE_X, BASE_Y);
        gfx_SwapDraw();

        while(!kb_IsDown(kb_KeyClear)) kb_Scan();
        while(kb_IsDown(kb_KeyClear)) kb_Scan();
        return;
    }

    selection = menu("Load from appvar:", options, num_vars);

    if(!selection) return;

    if(!appvar_to_vram(names[selection - 1])) return;
    vram_to_boot_code();
}

void menu_verify_current(void) {
    //todo: verify current bootcode
}

void menu_verify_appvar(void) {
    //todo: verify appvar
}

void menu_disable_verification(void) {
    //todo: handle errors
    void *location = get_mod_location();
    if(!location) return;
    patch(location, patch_data, unpatch_data, PATCH_SIZE);
}

void menu_enable_verification(void) {
    //todo: handle errors
    void *location = get_mod_location();
    if(!location) return;
    patch(location, unpatch_data, patch_data, PATCH_SIZE);
}

