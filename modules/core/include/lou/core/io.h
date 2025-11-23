#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include <stdio.h>
#include <stdbool.h>

bool lou_file_read_all(lou_mempool_t *mempool, FILE *stream, lou_slice_t *output);
