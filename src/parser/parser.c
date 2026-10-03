#include "parser.h"

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

        if (fseek(f, -4, SEEK_CUR) != 0)
            return PARSE_IO_ERROR;

        switch (block_type) {

        case IDB: {
            idb_t idb = {0};

            result = read_idb(f, &idb);
            if (result != PARSE_OK)
                return result;

            idb_t *tmp = realloc(pcapng->interfaces, (pcapng->interface_count + 1) * sizeof(idb_t));

            if (tmp == NULL)
                return PARSE_OOM;

            pcapng->interfaces = tmp;

            pcapng->interfaces[pcapng->interface_count] = idb;
            pcapng->interface_count++;

            break;
        }

        case EPB: {
            epb_t epb = {0};

            result = read_epb(f, &epb);
            if (result != PARSE_OK)
                return result;

            epb_t *tmp = realloc(pcapng->packets, (pcapng->packet_count + 1) * sizeof(epb_t) );

            if (tmp == NULL)
                return PARSE_OOM;

            pcapng->packets = tmp;

            pcapng->packets[pcapng->packet_count] = epb;
            pcapng->packet_count++;

            break;
        }

        default:
            return PARSE_INVALID;
        }
    }

    return PARSE_OK;
}

void free_pcapng(pcapng_t *pcapng) {
    for (size_t i = 0; i < pcapng->shb.options_count; i++)
        free(pcapng->shb.options[i].value);

    free(pcapng->shb.options);

    for (size_t i = 0; i < pcapng->interface_count; i++) {
        for (size_t j = 0; j < pcapng->interfaces[i].options_count; j++) {
            free(pcapng->interfaces[i].options[j].value);
        }

        free(pcapng->interfaces[i].options);
    }

    free(pcapng->interfaces);

    for (size_t i = 0; i < pcapng->packet_count; i++) {
        free(pcapng->packets[i].packet_data);

        for (size_t j = 0; j < pcapng->packets[i].options_count; j++) {
            free(pcapng->packets[i].options[j].value);
        }

        free(pcapng->packets[i].options);
    }

    free(pcapng->packets);
}