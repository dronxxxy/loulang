#pragma once

#include "core/mempool.h"
#include "core/slice.h"
#include <stdio.h>
#include <stdbool.h>

bool file_read_all(mempool_t *mempool, FILE *stream, slice_t *output);
