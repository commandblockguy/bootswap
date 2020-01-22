#ifndef FLASH_VERSIONS_H
#define FLASH_VERSIONS_H

#include <stdint.h>
#include <graphx.h>

struct version_data {
    uint8_t ld_1;
    uint8_t very_major;
    uint8_t ld_2;
    uint8_t major;
    uint8_t ret_1;
    uint8_t ld_3;
    uint8_t minor;
    uint8_t ret_2;
    uint8_t ld_4;
    uint8_t build_upper;
    uint8_t ld_5;
    uint8_t build_lower;
};

struct version_call {
    uint8_t jp;
    struct version_data *address;
};

const struct version_call *version_monstrosity = (void*)(gfx_vram + 0x80);

#define version_in_vram ((struct version_data *)&gfx_vram[(uint24_t)version_monstrosity->address])

struct version_number {
    uint8_t bootMajorVersion;
    uint8_t bootMinorVersion;
    uint8_t bootRevisionVersion;
    uint24_t bootBuildVersion;
};

struct version {
    struct version_number number;
    void *verification_location;
};

#define NUM_VERSIONS 3

extern const struct version versions[NUM_VERSIONS];

void *get_mod_location(void);

#define PATCH_SIZE 5
extern const uint8_t   patch_data[PATCH_SIZE];
extern const uint8_t unpatch_data[PATCH_SIZE];

#endif //FLASH_VERSIONS_H
