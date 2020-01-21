#ifndef BOOTSWAP_UI_H
#define BOOTSWAP_UI_H

struct menu_option {
    void (*function)(void);
    char *str;
};

/* Generic menu function */
uint8_t menu(const char *name, const struct menu_option *options, uint8_t num_options);
bool prompt(const char *name, const char *text);

void main_menu(void);

/* Options in the main menu */

void menu_backup(void);
void menu_install(void);
void menu_verify_current(void);
void menu_verify_appvar(void);
void menu_disable_verification(void);
void menu_enable_verification(void);

#endif //BOOTSWAP_UI_H
