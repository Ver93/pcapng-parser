#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "types.h"
#include "utils.h"
#include "validation.h"

int debug = 0;

endianness_t file_endianness;

#define DEBUG_PRINT(...) \
    do { if (debug) printf(__VA_ARGS__); } while(0)

void read_shb(FILE *f, shb_t *shb){
    DEBUG_PRINT("\n[DEBUG] SHB BLOCK START\n");
    fread(&shb->block_type,         sizeof(shb->block_type),        1, f);
    fread(&shb->block_length,       sizeof(shb->block_length),      1, f);
    fread(&shb->byte_order_magic,   sizeof(shb->byte_order_magic),  1, f);
    fread(&shb->major_version,      sizeof(shb->major_version),     1, f);
    fread(&shb->minor_version,      sizeof(shb->minor_version),     1, f);
    fread(&shb->section_length,     sizeof(shb->section_length),    1, f);

    if (shb->byte_order_magic == 0x1A2B3C4D) {
        file_endianness = IS_LITTLE_ENDIAN;
    } else if (shb->byte_order_magic == 0x4D3C2B1A) {
        file_endianness = IS_BIG_ENDIAN;
    } else {
        printf("SHB: invalid byte_order_magic\n");
        exit(1);
    }

    if (file_endianness != get_system_endianness()) {
        shb->block_type      = swap32(shb->block_type);
        shb->block_length    = swap32(shb->block_length);
        shb->major_version   = swap16(shb->major_version);
        shb->minor_version   = swap16(shb->minor_version);
        shb->section_length  = swap64(shb->section_length);
    }

    DEBUG_PRINT("[DEBUG] SHB block_type: 0x%08X\n", shb->block_type);
    DEBUG_PRINT("[DEBUG] SHB block_length: %u\n", shb->block_length);
    DEBUG_PRINT("[DEBUG] SHB byte_order_magic: 0x%08X\n", shb->byte_order_magic);
    DEBUG_PRINT("[DEBUG] File endianness: %s\n", file_endianness == IS_LITTLE_ENDIAN ? "Little endian" : "Big endian");
    DEBUG_PRINT("[DEBUG] SHB version: %u.%u\n", shb->major_version, shb->minor_version);
    DEBUG_PRINT("[DEBUG] SHB section_length: %llu\n", (unsigned long long)shb->section_length);

    size_t header_size = sizeof(shb->block_type) + sizeof(shb->block_length) +
                         sizeof(shb->byte_order_magic) + sizeof(shb->major_version) +
                         sizeof(shb->minor_version) + sizeof(shb->section_length);
    size_t options_size = shb->block_length - header_size - 4;

    if (options_size > 0) {
        DEBUG_PRINT("[DEBUG] SHB options_size: %zu bytes\n", options_size);

        while (1) {
            options_t opt;
            memset(&opt, 0, sizeof(opt));

            fread(&opt.code, sizeof(opt.code), 1, f);
            fread(&opt.length, sizeof(opt.length), 1, f);

            if (file_endianness != get_system_endianness()) {
                opt.code   = swap16(opt.code);
                opt.length = swap16(opt.length);
            }

            if (opt.code == 0 && opt.length == 0)
                break;

            DEBUG_PRINT("[DEBUG] SHB option code=%u length=%u\n",
                        opt.code, opt.length);

            opt.value = malloc(opt.length);
            fread(opt.value, opt.length, 1, f);

            shb->options_count++;
            shb->options = realloc(shb->options, shb->options_count * sizeof(options_t));
            shb->options[shb->options_count - 1] = opt;

            int padding = (4 - (opt.length % 4)) % 4;
            fseek(f, padding, SEEK_CUR);
        }
    }

    fread(&shb->block_footer, sizeof(shb->block_footer), 1, f);

    // Swap footer
    if (file_endianness != get_system_endianness())
        shb->block_footer = swap32(shb->block_footer);

    DEBUG_PRINT("[DEBUG] SHB block_footer: 0x%08X\n", shb->block_footer);
    DEBUG_PRINT("[DEBUG] SHB END\n");
}

void read_idb(FILE *f, idb_t *idb){
    DEBUG_PRINT("\n[DEBUG] IDB BLOCK START\n");
    fread(&idb->block_type,         sizeof(idb->block_type),        1, f);
    fread(&idb->block_length,       sizeof(idb->block_length),      1, f);
    fread(&idb->link_type,          sizeof(idb->link_type),         1, f);
    fread(&idb->reserved,           sizeof(idb->reserved),          1, f);
    fread(&idb->snap_length,        sizeof(idb->snap_length),       1, f);

    // Swap IDB header
    if (file_endianness != get_system_endianness()) {
        idb->block_type   = swap32(idb->block_type);
        idb->block_length = swap32(idb->block_length);
        idb->link_type    = swap16(idb->link_type);
        idb->reserved     = swap16(idb->reserved);
        idb->snap_length  = swap32(idb->snap_length);
    }

    DEBUG_PRINT("[DEBUG] IDB block_type: 0x%08X\n", idb->block_type);
    DEBUG_PRINT("[DEBUG] IDB block_length: %u\n", idb->block_length);
    DEBUG_PRINT("[DEBUG] IDB link_type: %u\n", idb->link_type);
    DEBUG_PRINT("[DEBUG] IDB snap_length: %u\n", idb->snap_length);

    size_t header_size = sizeof(idb->block_type) + sizeof(idb->block_length) +
                         sizeof(idb->link_type) + sizeof(idb->reserved) +
                         sizeof(idb->snap_length);

    size_t options_size = idb->block_length - header_size - 4;

    if (options_size > 0) {
        DEBUG_PRINT("[DEBUG] IDB options_size: %zu bytes\n", options_size);

        while (1) {
            options_t opt;
            memset(&opt, 0, sizeof(opt));

            fread(&opt.code, sizeof(opt.code), 1, f);
            fread(&opt.length, sizeof(opt.length), 1, f);

            if (file_endianness != get_system_endianness()) {
                opt.code   = swap16(opt.code);
                opt.length = swap16(opt.length);
            }

            if (opt.code == 0 && opt.length == 0)
                break;

            DEBUG_PRINT("[DEBUG] IDB option code=%u length=%u\n",
                        opt.code, opt.length);

            opt.value = malloc(opt.length);
            fread(opt.value, opt.length, 1, f);

            idb->options_count++;
            idb->options = realloc(idb->options, idb->options_count * sizeof(options_t));
            idb->options[idb->options_count - 1] = opt;

            int padding = (4 - (opt.length % 4)) % 4;
            fseek(f, padding, SEEK_CUR);
        }
    }

    fread(&idb->block_footer, sizeof(idb->block_footer), 1, f);

    if (file_endianness != get_system_endianness())
        idb->block_footer = swap32(idb->block_footer);

    DEBUG_PRINT("[DEBUG] IDB block_footer: 0x%08X\n", idb->block_footer);
    DEBUG_PRINT("[DEBUG] IDB END\n");
}

void read_epb(FILE *f, epb_t *epb){
    DEBUG_PRINT("\n[DEBUG] EPB BLOCK START\n");
    fread(&epb->block_type,             4, 1, f);
    fread(&epb->block_length,           4, 1, f);
    fread(&epb->interface_id,           4, 1, f);
    fread(&epb->timestamp_high,         4, 1, f);
    fread(&epb->timestamp_low,          4, 1, f);
    fread(&epb->captured_packet_length, 4, 1, f);
    fread(&epb->original_packet_length, 4, 1, f);

    if (file_endianness != get_system_endianness()) {
        epb->block_type             = swap32(epb->block_type);
        epb->block_length           = swap32(epb->block_length);
        epb->interface_id           = swap32(epb->interface_id);
        epb->timestamp_high         = swap32(epb->timestamp_high);
        epb->timestamp_low          = swap32(epb->timestamp_low);
        epb->captured_packet_length = swap32(epb->captured_packet_length);
        epb->original_packet_length = swap32(epb->original_packet_length);
    }

    DEBUG_PRINT("[DEBUG] EPB block_type:                %u\n", epb->block_type);
    DEBUG_PRINT("[DEBUG] EPB block_length:              %u\n", epb->block_length);
    DEBUG_PRINT("[DEBUG] EPB interface_id:              %u\n", epb->interface_id);
    DEBUG_PRINT("[DEBUG] EPB captured_packet_length:    %u\n", epb->captured_packet_length);
    DEBUG_PRINT("[DEBUG] EPB original_packet_length:    %u\n", epb->original_packet_length);

    epb->packet_data = malloc(epb->captured_packet_length);
    fread(epb->packet_data, epb->captured_packet_length, 1, f);

    int padding = (4 - (epb->captured_packet_length % 4)) % 4;
    if (padding > 0) fseek(f, padding, SEEK_CUR);

    fread(&epb->block_footer, 4, 1, f);

    if (file_endianness != get_system_endianness())
        epb->block_footer = swap32(epb->block_footer);

    DEBUG_PRINT("[DEBUG] EPB block_footer:              %u\n", epb->block_footer);
    DEBUG_PRINT("[DEBUG] EPB END\n");
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pcapng_file> [--debug]\n", argv[0]);
        return 1;
    }

    if (argc >= 3 && strcmp(argv[2], "--debug") == 0) {
        debug = 1;
        printf("[DEBUG] Debug mode enabled\n");
    }

    FILE *fp = fopen(argv[1], "rb");

    if (!fp) {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }

    shb_t section_header_block = {0};
    idb_t interface_description_block = {0};
    epb_t enhanced_packet_block = {0};

    read_shb(fp, &section_header_block);
    if (validate_shb(&section_header_block) != 0) return 1;

    read_idb(fp, &interface_description_block);
    if (validate_idb(&interface_description_block) != 0) return 1;

    uint32_t epb_count = 0;

    while (1) {
        uint32_t block_type;

        if (fread(&block_type, sizeof(block_type), 1, fp) != 1) {
            DEBUG_PRINT("[DEBUG] EOF reached, stopping EPB loop.\n");
            break;
        }

        if (file_endianness != get_system_endianness())
            block_type = swap32(block_type);

        if (block_type != 0x00000006) {
            DEBUG_PRINT("[DEBUG] Non-EPB block encountered, stopping EPB loop.\n");
            break;
        }

        fseek(fp, -4, SEEK_CUR);

        read_epb(fp, &enhanced_packet_block);
        epb_count++;
        if (validate_epb(&enhanced_packet_block) != 0) {
            DEBUG_PRINT("[DEBUG] EPB validation failed.\n");
            break;
        }
    }

    DEBUG_PRINT("\n==================== PCAPNG SUMMARY ====================\n");
    DEBUG_PRINT("File endianness:                   %s\n", file_endianness == IS_LITTLE_ENDIAN ? "Little endian" : "Big endian");
    DEBUG_PRINT("Section Header Block (SHB):        OK\n");
    DEBUG_PRINT("  SHB options:                     %zu\n", section_header_block.options_count);
    DEBUG_PRINT("Interface Description Block (IDB): OK\n");
    DEBUG_PRINT("  IDB options:                     %zu\n", interface_description_block.options_count);
    DEBUG_PRINT("Enhanced Packet Blocks (EPB):      %u\n", epb_count);
    DEBUG_PRINT("=========================================================\n\n");



    for (size_t i = 0; i < section_header_block.options_count; i++)
        free(section_header_block.options[i].value);
    free(section_header_block.options);

    for (size_t i = 0; i < interface_description_block.options_count; i++)
        free(interface_description_block.options[i].value);
    free(interface_description_block.options);

    for (size_t i = 0; i < enhanced_packet_block.options_count; i++)
        free(enhanced_packet_block.options[i].value);
    free(enhanced_packet_block.options);

    free(enhanced_packet_block.packet_data);

    fclose(fp);
    return 0;
}
