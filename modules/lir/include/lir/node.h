#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"

typedef struct lir_node_t lir_node_t;
typedef struct lir_const_t lir_const_t;
typedef struct lir_type_t lir_type_t;

lir_node_t *lir_node_new_func(lou_mempool_t *mempool);
lir_node_t *lir_node_new_return(lou_mempool_t *mempool, lir_node_t *control, lir_node_t *value);
lir_node_t *lir_node_new_const(lou_mempool_t *mempool, lir_const_t *constant);
lir_node_t *lir_node_new_extern(lou_mempool_t *mempool, lir_type_t *type, lou_slice_t name);
lir_node_t *lir_node_new_call(lou_mempool_t *mempool, lir_node_t *control, lir_node_t *memory, lir_node_t *callable, ...);

