#include <tice.h>
#include <string.h>
#include "versions.h"

const struct version versions[NUM_VERSIONS] = {
    {{0x05, 0x00, 0x00, 0x59}, {(void*)0x5907,          NULL, (void*)0x5CB6},   910250,  3340696},
    {{0x05, 0x01, 0x05, 0x0e}, {(void*)0x5CC8, (void*)0x0000, NULL, (void*)0x6077}, 16288302, 11582739},
    {{0x05, 0x03, 0x01, 0x32}, {(void*)0x5ECF, NULL, (void*)0x647B, (void*)0x6401},  1825178,  9998982}
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
