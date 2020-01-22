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
#define TEXT_HEIGHT 8

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

            while(kb_IsDown(kb_KeyEnter) ||
                  kb_IsDown(kb_Key2nd) ||
                  kb_IsDown(kb_KeyClear)) kb_Scan();
            return selected + 1;
        }
    }
}

bool text_entry(char *str, uint8_t length, uint8_t y) {
    const char getkey_letters[] = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?\0VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
    const char getkey_numeric[] = "\0\0\0\0\0\0\0\0\0\0+-*/^\0\0-369)\0\0\0.258(\0\0\0000147,\0\0\0\0>\0\0\0\0\0\0\0\0\0\0\0\0\0";
    enum entry_mode {
        UPPERCASE,
        LOWERCASE,
        NUMERIC
    };
    uint8_t pos = 0; // the position new characters will be inserted into
    uint8_t end = 0; // the index of the null byte
    uint8_t mode = UPPERCASE;
    bool key_pressed = true;
    uint8_t key_value;
    uint8_t blink_value = 0;
    uint8_t i;
#define SLOT_WIDTH 8
#define SLOT_GAP 3
    uint24_t base_x = (LCD_WIDTH - (SLOT_WIDTH * (length - 1) + SLOT_GAP * (length - 2))) / 2;

    memset(str, 0, length);

    while(true) {
        kb_Scan();

        key_value = os_GetCSC();

        if(!key_pressed) {
            if(kb_IsDown(kb_KeyClear)) {
                return false;
            }
            if(kb_IsDown(kb_KeyEnter)) {
                return true;
            }

            if(kb_IsDown(kb_KeyRight) && pos < end) pos++;
            if(kb_IsDown(kb_KeyLeft) && pos > 0) pos--;

            if(kb_IsDown(kb_KeyDel) && pos != end) {
                memmove(&str[pos], &str[pos+1], end - pos);
                end--;
            }

            if(kb_IsDown(kb_KeyAlpha)) {
                mode++;
                if(mode > 2) mode = 0;
            }
        }

        key_pressed = false;
        for(i = 1; i <= 7; i++) {
            if(kb_Data[i]) key_pressed = true;
        }

        if(key_value && end < length - 1) {
            char ch;
            if(mode == NUMERIC) {
                ch = getkey_numeric[key_value];
            } else {
                ch = getkey_letters[key_value];
                if(mode == LOWERCASE && ch >= 'A' && ch <= 'Z') {
                    // convert to lowercase
                    ch -= ('A' - 'a');
                }
            }

            if(ch) {
                memmove(&str[pos + 1], &str[pos], end - pos + 1);
                str[pos] = ch;
                pos++;
                end++;
            }
        }

        gfx_SetColor(BG_COLOR);
        gfx_FillRectangle_NoClip(0, y, LCD_WIDTH, 2 * TEXT_HEIGHT + 3);

        for(i = 0; i < length - 1; i++) {
            gfx_SetColor(TEXT_COLOR);
            if(i < end) {
                gfx_SetTextXY(base_x + i * (SLOT_WIDTH + SLOT_GAP), y);
                gfx_PrintChar(str[i]);
            }
            if(i == pos) {
                gfx_SetTextXY(base_x + i * (SLOT_WIDTH + SLOT_GAP), y + TEXT_HEIGHT + 3);
                switch(mode) {
                    default:
                    case UPPERCASE:
                        gfx_PrintChar('A');
                        break;
                    case LOWERCASE:
                        gfx_PrintChar('a');
                        break;
                    case NUMERIC:
                        gfx_PrintChar('1');
                        break;
                }
                if (blink_value & 32) {
                    gfx_SetColor(SELECTED_COLOR);
                }
            }
            gfx_HorizLine(base_x + i * (SLOT_WIDTH + SLOT_GAP), y + TEXT_HEIGHT, SLOT_WIDTH);
        }

        blink_value++;
        gfx_BlitBuffer();
    }
}

void print_wrapped_text(char *str, uint24_t left_margin, uint8_t top_margin, uint24_t right_margin) {
    char *word = str;
    char *current = str;

    gfx_SetTextXY(left_margin, top_margin);

    while(true) {
        uint8_t width = 0;
        for(; *current != ' ' && *current != 0; current++) {
            width += gfx_GetCharWidth(*current);
        }

        if(gfx_GetTextX() + width > right_margin) {
            gfx_SetTextXY(left_margin, gfx_GetTextY() + LINE_SPACING);
        }

        for(; word <= current; word++) {
            gfx_PrintChar(*word);
        }

        if(*current == 0) return;
        current++;
    }

}

void message(char *title, char *str) {
    gfx_FillScreen(BG_COLOR);
    gfx_SetTextFGColor(TEXT_COLOR);
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY(title, BASE_X, TITLE_Y);
    gfx_SetTextScale(1, 1);
    print_wrapped_text(str, BASE_X, BASE_Y, LCD_WIDTH - BASE_X);
    gfx_SwapDraw();

    while(kb_IsDown(kb_KeyClear) || kb_IsDown(kb_KeyEnter)) kb_Scan();
    while(!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyEnter)) kb_Scan();
    while(kb_IsDown(kb_KeyClear) || kb_IsDown(kb_KeyEnter)) kb_Scan();
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
    char str[8];

    gfx_FillScreen(BG_COLOR);

    gfx_PrintStringXY("Appvar to backup to:", BASE_X, BASE_Y);

    if(!text_entry(str, 8, LCD_HEIGHT / 3)) return;

    if(!strlen(str)) {
        message("Error:", "Invalid file name.");
        return;
    }

    boot_code_to_vram();
    if(!vram_to_appvar(str)) {
        message("Error:", "Failed to create backup. Try deleting or archiving programs to make space.");
        return;
    };

    message("Success", "Backup complete.");
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
        message("Error:", "No backups found.");
    }

    selection = menu("Load from appvar:", options, num_vars);

    if(!selection) return;

    if(!appvar_to_vram(names[selection - 1])) {
        message("Error:", "Failed to read backup appvars.");
    }
    vram_to_boot_code();

    message("Success", "Bootcode installed successfully.");
}

void menu_verify_current(void) {
    //todo: verify current bootcode
    message("Error:", "Verification is not yet implemented.");
}

void menu_verify_appvar(void) {
    //todo: verify appvar
    message("Error:", "Verification is not yet implemented.");
}

void menu_disable_verification(void) {
    void *location = get_mod_location();
    if(!location) {
        message("Error:", "Unable to determine which memory location to patch for this boot code version.");
        return;
    }

    if(!patch(location, patch_data, unpatch_data, PATCH_SIZE)) {
        message("Error:", "Patch not applied - the location to be overwritten contained unexpected data.");
    }

    message("Success", "OS verification disabled.");
}

void menu_enable_verification(void) {
    void *location = get_mod_location();
    if(!location) {
        message("Error:",
                "Unable to find patch location for this bootcode version");
        return;
    }

    if(!patch(location, unpatch_data, patch_data, PATCH_SIZE)) {
        message("Error:", "Patch not applied - the location to be overwritten contained unexpected data.");
    }

    message("Success", "OS verification re-enabled.");
}

