#pragma once

#include "lou/core/mempool.h"

typedef struct lir_node_t lir_node_t;
typedef struct lir_t lir_t;

lir_t *lir_new(lou_mempool_t *mempool);
void lir_add_func(lir_t *lir, lir_node_t *node);
void lir_add_extern(lir_t *lir, lir_node_t *node);
