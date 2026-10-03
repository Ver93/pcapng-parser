#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "utils.h"
#include "parser.h"
#include "validation.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pcapng_file> [--debug]\n", argv[0]);
        return 1;
    }

    if (argc >= 3 && strcmp(argv[2], "--debug") == 0) {
        debug = 1;
        DEBUG_PRINT("[DEBUG] Debug mode enabled\n");
    }

    FILE *fp = fopen(argv[1], "rb");

    if (!fp) {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }

    pcapng_t pcapng = {0};

    int result = parse_pcapng(fp, &pcapng);

    fclose(fp);

    if (result != PARSE_OK) {
        fprintf(stderr, "Error parsing PCAPNG file\n");
        free_pcapng(&pcapng);
        return 1;
    }

    DEBUG_PRINT("\n==================== PCAPNG SUMMARY ====================\n");
    DEBUG_PRINT("File endianness:                   %s\n", file_endianness == IS_LITTLE_ENDIAN ? "Little endian" : "Big endian");
    DEBUG_PRINT("Section Header Block (SHB):        OK\n");
    DEBUG_PRINT("  SHB options:                     %zu\n", pcapng.shb.options_count);
    DEBUG_PRINT("Interface Description Blocks:      %zu\n", pcapng.interface_count);
    DEBUG_PRINT("Enhanced Packet Blocks (EPB):      %zu\n", pcapng.packet_count);
    DEBUG_PRINT("=========================================================\n\n");

    free_pcapng(&pcapng);

    return 0;
}