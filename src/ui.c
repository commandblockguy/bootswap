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
#include "verification.h"
#include "gfx/text.h"

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

bool message(char *title, char *str) {
    gfx_FillScreen(BG_COLOR);
    gfx_SetTextFGColor(TEXT_COLOR);
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY(title, BASE_X, TITLE_Y);
    gfx_SetTextScale(1, 1);
    print_wrapped_text(str, BASE_X, BASE_Y, LCD_WIDTH - BASE_X);
    gfx_SwapDraw();

    while(kb_IsDown(kb_KeyClear) || kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd))
        kb_Scan();

    while(true) {
        kb_Scan();
        if(kb_IsDown(kb_KeyClear)) {
            while(kb_IsDown(kb_KeyClear)) kb_Scan();
            return false;
        }
        if(kb_IsDown(kb_KeyEnter)) {
            while(kb_IsDown(kb_KeyEnter)) kb_Scan();
            return true;
        }
        if(kb_IsDown(kb_Key2nd)) {
            while(kb_IsDown(kb_Key2nd)) kb_Scan();
            return true;
        }
    }
}

bool get_appvar_name(char *title, char* result) {
    struct menu_option options[10];
    char names[10][9];
    void *search_pos;
    uint8_t num_vars;
    uint8_t selection;

    for(search_pos = NULL, num_vars = 0; num_vars < 100; num_vars++) {
        char *name;
        options[num_vars].function = NULL;
        options[num_vars].str = names[num_vars];
        if(!(name = ti_Detect(&search_pos, "bootcode"))) break;
        strcpy(names[num_vars], name);
        // todo: maybe ignore appvars with missing data
    }

    if(!num_vars) {
        message("Error:", "No backup appvars found.");
        return false;
    }

    selection = menu(title, options, num_vars);

    if(!selection) return false;

    strcpy(result, names[selection - 1]);
    return true;
}

bool paperweight(char *str) {
    char buf[12];
    gfx_FillScreen(BG_COLOR);
    gfx_SetTextFGColor(TEXT_COLOR);
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("WARNING", BASE_X, TITLE_Y);
    gfx_SetTextScale(1, 1);
    print_wrapped_text(str, BASE_X, BASE_Y, LCD_WIDTH - BASE_X);
    text_entry(buf, 12, gfx_GetTextY() + 2 * TEXT_HEIGHT);

    return strcmp(buf, "paperweight") == 0 || strcmp(buf, "PAPERWEIGHT") == 0;
}

uint32_t old_screen_base;

void wait_screen(void) {
    uint8_t * const new_screen_base = gfx_vram + LCD_WIDTH * LCD_HEIGHT * 15 / 8;
    const uint24_t screen_size = LCD_WIDTH * LCD_HEIGHT / 8;
    uint8_t i;

    old_screen_base = lcd_UpBase;
    lcd_UpBase = (uint24_t)new_screen_base;
    /* Set to 1bpp mode and pixel endianness */
    lcd_Control = (lcd_Control & ~0xE) | 0x0400;
    memset(new_screen_base, 0, screen_size);

    /* Draw our sprite */
    for(i = 0; i < text_height; i++) {
        const uint24_t text_x = 133;
        const uint8_t text_y = (LCD_HEIGHT - text_height) / 2;
        memcpy(new_screen_base + text_x / 8 + (text_y + i) * LCD_WIDTH / 8, &text_data[text_width * i], text_width);
    }
}

void exit_wait_screen(void) {
    lcd_UpBase = old_screen_base;
    /* Reset to 8bpp and old endianness settings */
    lcd_Control = (lcd_Control & ~0x040E) | 0b110;
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

    wait_screen();
    boot_code_to_vram();
    if(!vram_to_appvar(str)) {
        exit_wait_screen();
        message("Error:", "Failed to create backup. Try deleting or archiving programs to make space.");
        return;
    };

    exit_wait_screen();
    message("Success", "Backup complete.");
}

void menu_install(void) {
    char name[9];
    uint24_t crc = 0;
    struct version_number version_number;
    const struct version *version;
    bool has_interrupt_handlers;
    bool has_calls;
    bool pre_rev_m;
    bool is_paperweight = false;

    if(!get_appvar_name("Load from appvar:", name)) return;

    wait_screen();
    if(!appvar_to_vram(name)) {
        exit_wait_screen();
        message("Error:", "Failed to read backup appvars.");
        return;
    }

    version_number.bootMajorVersion = version_in_vram->very_major;
    version_number.bootMinorVersion = version_in_vram->major;
    version_number.bootRevisionVersion = version_in_vram->minor;
    version_number.bootBuildVersion = version_in_vram->build_upper << 8 | version_in_vram->build_lower;

    version = get_version(&version_number);

    if(version) crc = crc24(gfx_vram, 0x020000);

    has_interrupt_handlers = verify_interrupt_handlers();
    has_calls = verify_boot_calls();
    pre_rev_m = verify_pre_m_version();

    exit_wait_screen();

    if(!has_interrupt_handlers) {
        if(paperweight("The bootcode you are attempting to install does not appear to have interrupt handlers. Are you sure that it is actually a valid bootcode? Continuing the installation process will almost definitely result in the calculator becoming permanently inoperable. Press clear to cancel the installation. If you are sure that you wish to proceed with the installation, enter the word \"PAPERWEIGHT\", which is what this calculator will become in thirty seconds."))
            is_paperweight = true;
        else {
            message("Cancelled", "Installation cancelled.");
            return;
        }
    }

    if(!has_calls) {
        if(paperweight("The bootcode you are attempting to install does not appear to have standard bootcode calls. Continuing the installation process will likely result in the calculator becoming permanently inoperable. Press clear to cancel the installation. If you are sure that you wish to proceed with the installation, enter the word \"PAPERWEIGHT\", which is what this calculator will become in thirty seconds."))
            is_paperweight = true;
        else {
            message("Cancelled", "Installation cancelled.");
            return;
        }
    }

    if(!pre_rev_m) {
        if(paperweight("You appear to be installing a boot code designed for a calculator with a hardware revision of M or higher. The hardware for these newer calculators is substantially different from the hardware on your calculator. Continuing the installation process will result in the calculator becoming permanently inoperable.  Press clear to cancel the installation. If you are sure that you wish to proceed with the installation, enter the word \"PAPERWEIGHT\", which is what this calculator will become in thirty seconds."))
            is_paperweight = true;
        else {
            message("Cancelled", "Installation cancelled.");
            return;
        }
    }

    if(!version) {
        if(!is_paperweight) {
            if(!message("Warning:", "The boot code version you are about to install is not a known version. As a result, it is not possible to verify that the boot code has not been modified. If you are sure that this is an unmodified boot code, please send the version number and CRC checksum (both found in the Verify Appvar menu) to commandblockguy1+bootversions@gmail.com. Press Enter or 2nd to continue with the installation, or clear to cancel.")) {
                message("Cancelled", "Installation cancelled.");
                return;
            }
        }
    } else {
        if(!is_paperweight && crc != version->crc_original && crc != version->crc_patched) {
            if(!message("Warning:", "The CRC checksum of the bootcode you are about to install does not match the checksum of the unmodified bootcode. This indicates that the bootcode has been modified or corrupted. Press Enter or 2nd to continue with the installation, or clear to cancel.")) {
                message("Cancelled", "Installation cancelled.");
                return;
            }
        }
    }

    wait_screen();

    if(!appvar_to_vram(name)) {
        exit_wait_screen();
        message("Error:", "Failed to read backup appvars.");
        return;
    }

    if(!vram_to_boot_code())
        if(!vram_to_boot_code())
            if(!vram_to_boot_code()) {
                exit_wait_screen();
                message("ERROR", "Bootcode failed to install - Device may have just bricked :(");
                return;
            }

    exit_wait_screen();
    message("Success", "Bootcode installed successfully.");
}

void menu_verify(void) {
    uint24_t crc;
    struct version_number version_number;
    const struct version *version;
    bool has_interrupt_handlers;
    bool has_calls;
    bool pre_rev_m;

    crc = crc24(gfx_vram, 0x020000);

    version_number.bootMajorVersion = version_in_vram->very_major;
    version_number.bootMinorVersion = version_in_vram->major;
    version_number.bootRevisionVersion = version_in_vram->minor;
    version_number.bootBuildVersion = version_in_vram->build_upper << 8 | version_in_vram->build_lower;

    version = get_version(&version_number);

    has_interrupt_handlers = verify_interrupt_handlers();
    has_calls = verify_boot_calls();
    pre_rev_m = verify_pre_m_version();

    exit_wait_screen();

    gfx_FillScreen(BG_COLOR);

    gfx_SetTextFGColor(TEXT_COLOR);

    gfx_PrintStringXY("Version: ", BASE_X, BASE_Y);
    gfx_PrintUInt(version_number.bootMajorVersion, 1);
    gfx_PrintChar('.');
    gfx_PrintUInt(version_number.bootMinorVersion, 1);
    gfx_PrintChar('.');
    gfx_PrintUInt(version_number.bootRevisionVersion, 1);
    gfx_PrintChar('.');
    gfx_PrintUInt(version_number.bootBuildVersion, 4);

    gfx_PrintStringXY("CRC: ", BASE_X, BASE_Y + LINE_SPACING);
    gfx_PrintUInt(crc, 8);

    gfx_PrintStringXY("Known CRC: ", BASE_X, BASE_Y + LINE_SPACING * 2);
    if(version)
        gfx_PrintUInt(version->crc_original, 8);
    else {
        gfx_PrintString("Unknown version.");
    }
    gfx_PrintStringXY("Known patched CRC: ", BASE_X, BASE_Y + LINE_SPACING * 3);
    if(version)
        gfx_PrintUInt(version->crc_patched, 8);
    else {
        gfx_PrintString("Unknown version.");
    }

    if(!has_interrupt_handlers)
        gfx_PrintStringXY("Interrupt handlers missing!", BASE_X, BASE_Y + LINE_SPACING * 4);

    if(!has_calls)
        gfx_PrintStringXY("Bootcode calls missing!", BASE_X, BASE_Y + LINE_SPACING * 5);

    if(!has_calls || !has_interrupt_handlers || !pre_rev_m)
        gfx_PrintStringXY("Not suitable for installation.", BASE_X, BASE_Y + LINE_SPACING * 6);
    else
        gfx_PrintStringXY("Suitable for installation.", BASE_X, BASE_Y + LINE_SPACING * 6);

    gfx_SwapDraw();

    while(kb_IsDown(kb_KeyClear) || kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd)) kb_Scan();
    while(!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyEnter) && !kb_IsDown(kb_Key2nd)) kb_Scan();
}

void menu_verify_current(void) {
    wait_screen();
    boot_code_to_vram();
    menu_verify();
}

void menu_verify_appvar(void) {
    char name[9];

    if(!get_appvar_name("Verify appvar:", name)) return;

    wait_screen();
    if(!appvar_to_vram(name)) {
        exit_wait_screen();
        message("Error:", "Failed to read backup appvars.");
        return;
    }

    menu_verify();
}

void menu_disable_verification(void) {
    const struct version_number *version_number = (struct version_number*)&os_GetSystemInfo()->bootMajorVersion;
    const struct version *version = get_version(version_number);
    uint8_t i;

    if(!version) {
        message("Error:", "Cannot patch an unknown bootcode version.");
        return;
    }

    wait_screen();

    for(i = 0; i < NUM_PATCHES; i++) {
        void *location = version->patch_locations[i];
        struct patch *patch = &patches[i];
        if(location == NULL) continue;
        if(!apply_patch(location, patch->patched_data, patch->unpatched_data, patch->size)) {
            exit_wait_screen();
            message("Error:", "Patch not applied - the location to be overwritten contained unexpected data.");
            return;
        }
    }

    exit_wait_screen();
    message("Success", "OS verification disabled.");
}

void menu_enable_verification(void) {
    const struct version_number *version_number = (struct version_number*)&os_GetSystemInfo()->bootMajorVersion;
    const struct version *version = get_version(version_number);
    uint8_t i;

    if(!version) {
        message("Error:", "Cannot patch an unknown bootcode version.");
        return;
    }

    wait_screen();

    for(i = 0; i < NUM_PATCHES; i++) {
        void *location = version->patch_locations[i];
        const struct patch *patch = &patches[i];
        if(!location) continue;
        if(!apply_patch(location, patch->unpatched_data, patch->patched_data, patch->size)) {
            exit_wait_screen();
            message("Error:", "Patch not applied - the location to be overwritten contained unexpected data.");
            return;
        }
    }

    exit_wait_screen();
    message("Success", "OS verification re-enabled.");
}

