#ifndef VALIDATION_H
#define VALIDATION_H

#include <stdio.h>
#include "types.h"

int validate_shb(const shb_t *shb);
int validate_idb(const idb_t *idb);
int validate_epb(const epb_t *epb);

#endif