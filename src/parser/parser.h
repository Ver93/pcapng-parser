#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "blocks.h"
#include "validation.h"
#include "utils.h"
#include "types.h"

int parse_pcapng(FILE *f, pcapng_t *pcapng);

void free_pcapng(pcapng_t *pcapng);

#endif