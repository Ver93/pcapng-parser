#include "validation.h"

int validate_shb(const shb_t *shb){
    if (shb->block_type != 0x0A0D0D0A){
        printf("SHB: invalid block_type\n");
        return 1;
    }
    if (shb->block_length < 28 || shb->block_length > 10*1024*1024){
        printf("SHB: invalid block_length\n");
        return 1;
    }
    if (shb->byte_order_magic != 0x1A2B3C4D && shb->byte_order_magic != 0x4D3C2B1A){
        printf("SHB: invalid byte_order_magic\n");
        return 1;
    }
    if (shb->major_version != 1 || shb->minor_version != 0){
        printf("SHB: invalid version\n");
        return 1;
    }
    if (shb->block_footer != shb->block_length){
        printf("SHB: invalid block_footer\n");
        return 1;
    }
    if (shb->section_length == 0){
        printf("SHB: invalid section_length\n");
        return 1;
    }
    return 0;
}

int validate_idb(const idb_t *idb){
    if (idb->block_type != 0x00000001){
        printf("IDB: invalid block_type\n");
        return 1;
    }
    if (idb->block_length < 20 || idb->block_length > 10*1024*1024){
        printf("IDB: invalid block_length\n");
        return 1;
    }
    if (idb->link_type == 0){
        printf("IDB: invalid link_type\n");
        return 1;
    }
    if (idb->snap_length == 0){
        printf("IDB: invalid snap_length\n");
        return 1;
    }
    if (idb->block_footer != idb->block_length){
        printf("IDB: invalid block_footer\n");
        return 1;
    }
    return 0;
}


int validate_epb(const epb_t *epb){

    if (epb->block_type != 0x00000006){
        printf("EPB: invalid block_type\n");
        return 1;
    }
    if (epb->block_length < 32 || epb->block_length > 10*1024*1024){
        printf("EPB: invalid block_length\n");
        return 1;
    }
    if (epb->interface_id == 0xFFFFFFFF){
        printf("EPB: invalid interface_id\n");
        return 1;
    }
    if (epb->captured_packet_length == 0){
        printf("EPB: invalid captured_packet_length\n");
        return 1;
    }
    if (epb->original_packet_length == 0){
        printf("EPB: invalid original_packet_length\n");
        return 1;
    }
    if (epb->block_footer != epb->block_length){
        printf("EPB: invalid block_footer\n");
        return 1;
    }
    if (epb->captured_packet_length > epb->original_packet_length){
        printf("EPB: captured > original length\n");
        return 1;
    }

    return 0;
}