#ifndef BLOCKS_H
#define BLOCKS_H

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "types.h"

int read_shb(FILE *f, shb_t *shb);
int read_idb(FILE *f, idb_t *idb);
int read_epb(FILE *f, epb_t *epb);

#endif