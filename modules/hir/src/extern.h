#pragma once

#include "lou/core/mempool.h"
#include "lou/hir/extern.h"

lou_hir_extern_t *lou_hir_extern_new(lou_mempool_t *mempool, lou_hir_type_t *type, lou_slice_t name);
