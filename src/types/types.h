#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    IS_BIG_ENDIAN = 0,
    IS_LITTLE_ENDIAN = 1
} endianness_t;

typedef enum {
    PARSE_OK = 0,
    PARSE_EOF,
    PARSE_TRUNCATED,
    PARSE_INVALID,
    PARSE_IO_ERROR,
    PARSE_OOM
} parse_result_t;

typedef enum {
    SHB = 0x0A0D0D0A,
    IDB = 0x00000001,
    // PB  = 0x00000002,
    // SPB = 0x00000003,
    // NRB = 0x00000004,
    // ISB = 0x00000005,
    EPB = 0x00000006,
    // DSB = 0x0000000A,

    // IRIG_TS = 0x00000009,

    // CB0 = 0x00000BAD,
    // CB1 = 0x40000BAD
} blocks_t;

typedef struct {
    uint16_t code;
    uint16_t length;
    uint8_t *value;
} options_t;

typedef struct {
    uint32_t block_type;
    uint32_t block_length;
    uint32_t byte_order_magic;
    uint16_t major_version;
    uint16_t minor_version;
    uint64_t section_length;
    options_t *options;
    size_t options_count;
    uint32_t block_footer;
} shb_t;

typedef struct {
    uint32_t block_type;
    uint32_t block_length;
    uint16_t link_type;
    uint16_t reserved;
    uint32_t snap_length;
    options_t *options;
    size_t options_count;
    uint32_t block_footer;
} idb_t;

typedef struct {
    uint32_t block_type;
    uint32_t block_length;
    uint32_t interface_id;
    uint32_t timestamp_high;
    uint32_t timestamp_low;
    uint32_t captured_packet_length;
    uint32_t original_packet_length;
    uint8_t *packet_data;
    options_t *options;
    size_t options_count;
    uint32_t block_footer;
} epb_t;

typedef struct {
    shb_t shb;

    idb_t *interfaces;
    size_t interface_count;

    epb_t *packets;
    size_t packet_count;
    
} pcapng_t;

#endif