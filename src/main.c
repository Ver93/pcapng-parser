#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "types.h"
#include "utils.h"
#include "blocks.h"
#include "validation.h"

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
