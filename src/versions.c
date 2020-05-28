#include <tice.h>
#include <string.h>
#include "versions.h"

const struct version versions[NUM_VERSIONS] = {
    {{5,0,0,82}, {(void*)0x58FC, NULL,          NULL,          (void*)0x5CAB},  1339699,  2079573},
    {{5,0,0,89}, {(void*)0x5907, NULL,          NULL,          (void*)0x5CB6},   910250,  5941963},
    {{5,1,5,14}, {(void*)0x5CC8, (void*)0x60DE, (void*)0x60E4, (void*)0x6077}, 16288302, 14412277},
    {{5,3,1,50}, {(void*)0x5ECF, NULL,          (void*)0x647B, (void*)0x6401},  1825178, 10083338},
    {{5,3,6,17}, {(void*)0x62C0, NULL,          (void*)0x686C, (void*)0x67F2},  4315312, 10417503},
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
