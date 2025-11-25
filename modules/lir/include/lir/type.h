#pragma once

#include "lou/core/mempool.h"
#include <stdbool.h>

typedef enum {
  LIR_TYPE_INT_8,
  LIR_TYPE_INT_16,
  LIR_TYPE_INT_32,
  LIR_TYPE_INT_64,
} lir_type_int_size_t;

typedef struct lir_type_t lir_type_t;

lir_type_t *lir_type_new_func(lou_mempool_t *mempool, lir_type_t **args, lir_type_t *returns);
lir_type_t *lir_type_new_int(lou_mempool_t *mempool, lir_type_int_size_t size, bool is_signed);
