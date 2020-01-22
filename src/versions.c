#include <tice.h>
#include <string.h>
#include "versions.h"

const struct version versions[NUM_VERSIONS] = {
    {{0x05, 0x00, 0x00, 0x59}, (void*)0x5907,   910250,  3340696},
    {{0x05, 0x01, 0x05, 0x0e}, (void*)0x5CC8, 16288302, 11582739},
    {{0x05, 0x03, 0x01, 0x32}, (void*)0x5ECF,  1825178,  9998982}
};

const struct version *get_version(const struct version_number *version_number) {
    uint8_t i;
    for(i = 0; i < NUM_VERSIONS; i++) {
        const struct version_number *current = &versions[i].number;
        if(memcmp(current, version_number, sizeof(struct version_number)) == 0) {
            return &versions[i];
        }
    }
    return NULL;
}

/* ld hl,1 \ ret */
const uint8_t   patch_data[PATCH_SIZE] = {0x21, 0x01, 0x00, 0x00, 0xC9};
const uint8_t unpatch_data[PATCH_SIZE] = {0xDD, 0xE5, 0xDD, 0x21, 0x00};
