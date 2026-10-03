#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "blocks.h"
#include "validation.h"
#include "utils.h"

int parse_pcapng(FILE *f, pcapng_t *pcapng)
{
    int result;

    result = read_shb(f, &pcapng->shb);
    if (result != PARSE_OK)
        return result;

    while (1) {
        uint32_t block_type;

        if (fread(&block_type, sizeof(block_type), 1, f) != 1)
            break;

        if (file_endianness != get_system_endianness())
            block_type = swap32(block_type);

        fseek(f, -4, SEEK_CUR);

        switch (block_type) {

        case 0x00000001: {
            idb_t idb = {0};

            result = read_idb(f, &idb);
            if (result != PARSE_OK)
                return result;
            break;
        }

        case 0x00000006: {
            epb_t epb = {0};

            result = read_epb(f, &epb);
            if (result != PARSE_OK)
                return result;

            break;
        }

        default:
            return PARSE_INVALID;
        }
    }

    return PARSE_OK;
}