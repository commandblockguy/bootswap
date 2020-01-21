#include <tice.h>
#include <string.h>
#include "versions.h"

const struct version versions[NUM_VERSIONS] = {
    {{0x05, 0x00, 0x00, 0x59}, (void*)0x5907},
    {{0x05, 0x01, 0x05, 0x0e}, (void*)0x5CC8},
    {{0x05, 0x03, 0x01, 0x32}, (void*)0x5ECF}
};

void *get_mod_location(void) {
    uint8_t i;
    const system_info_t *sys_info = os_GetSystemInfo();
    for(i = 0; i < NUM_VERSIONS; i++) {
        const struct version_number *current = &versions[i].number;
        if(memcmp(current, &sys_info->bootMajorVersion, sizeof(struct version_number)) == 0) {
            return versions[i].verification_location;
        }
    }
    return NULL;
}

/* ld hl,1 \ ret */
const uint8_t   patch_data[PATCH_SIZE] = {0x21, 0x01, 0x00, 0x00, 0xC9};
const uint8_t unpatch_data[PATCH_SIZE] = {0xDD, 0xE5, 0xDD, 0x21, 0x00};
