#pragma once

#include "lir/node.h"
#include "lou/core/mempool.h"
#include <stdint.h>

typedef struct lir_type_t lir_type_t;
typedef struct lir_const_t lir_const_t;

lir_const_t *lir_const_new_int(lou_mempool_t *mempool, lir_type_t *type, uint64_t value);
