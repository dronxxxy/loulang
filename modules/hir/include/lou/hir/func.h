#pragma once

#include "lou/core/mempool.h"
#include "lou/hir/mutability.h"
#include <stdbool.h>

typedef struct lou_hir_type_t lou_hir_type_t;
typedef struct lou_hir_code_t lou_hir_code_t;

typedef struct lou_hir_local_t {
  lou_hir_mutability_t mutability;
  lou_hir_type_t *type;
} lou_hir_local_t;

typedef struct lou_hir_func_t {
  lou_hir_local_t **locals;
  lou_hir_code_t *code;
} lou_hir_func_t;

bool lou_hir_func_valid(lou_hir_func_t *func);
void lou_hir_func_init(lou_hir_func_t *func, lou_hir_code_t *code);
lou_hir_local_t *lou_hir_func_local_add(lou_mempool_t *mempool, lou_hir_func_t *func, lou_hir_local_t local);

static inline lou_hir_func_t *lou_hir_func_new(lou_mempool_t *mempool) {
  lou_hir_func_t *func = LOU_MEMPOOL_ALLOC(mempool, lou_hir_func_t);
  func->locals = LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_local_t*);
  return func;
}
