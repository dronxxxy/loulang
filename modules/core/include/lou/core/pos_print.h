#pragma once

#include "lou/core/slice.h"
#include <stdio.h>

void lou_pos_print(FILE *stream, int color, lou_slice_t path, lou_slice_t file, lou_slice_t part);
