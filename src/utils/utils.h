#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>
#include "types.h"

extern int debug;
extern endianness_t file_endianness;

#define DEBUG_PRINT(...) \
    do { if (debug) printf(__VA_ARGS__); } while(0)

uint16_t swap16(uint16_t x);
uint32_t swap32(uint32_t x);
uint64_t swap64(uint64_t x);

endianness_t get_system_endianness();

void byte_to_hex(uint8_t *data, size_t len);
void hex_to_string(uint8_t *hex, size_t len);

#endif