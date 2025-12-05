#pragma once

#include "lou/core/mempool.h"

typedef struct lou_hir_func_t lou_hir_func_t;
typedef struct lou_hir_extern_t lou_hir_extern_t;
typedef struct lou_hir_type_t lou_hir_type_t;

typedef struct lou_hir_t {
  lou_mempool_t *mempool;
  lou_hir_func_t **functions;
  lou_hir_extern_t **externs;
} lou_hir_t;

lou_hir_t *lou_hir_new(lou_mempool_t *mempool);
lou_hir_func_t *lou_hir_func_add(lou_hir_t *ir, lou_hir_type_t *type);
lou_hir_extern_t *lou_hir_extern_add(lou_hir_t *ir, lou_hir_type_t *base_type, lou_slice_t name);

