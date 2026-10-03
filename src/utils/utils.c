#include "utils.h"

int debug = 0;
endianness_t file_endianness;

uint16_t swap16(uint16_t x) { return (x >> 8) | (x << 8); }
uint32_t swap32(uint32_t x) { return __builtin_bswap32(x); }
uint64_t swap64(uint64_t x) { return __builtin_bswap64(x); }

endianness_t get_system_endianness() {
    uint16_t num = 1;
    return (*(uint8_t *)&num == 1) ? IS_LITTLE_ENDIAN : IS_BIG_ENDIAN;
}

void byte_to_hex(uint8_t *data, size_t len) {
    for (size_t j = 0; j < len; j++)
        printf("0x%02X ", data[j]);
}

void hex_to_string(uint8_t *hex, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uint8_t c = hex[i];
        printf("%c", (c >= 32 && c <= 126) ? c : '.');
    }
    printf("\n");
}