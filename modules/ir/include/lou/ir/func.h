#pragma once

#include "lou/core/mempool.h"
#include "lou/ir/mutability.h"
#include <stdbool.h>

typedef struct lou_ir_type_t lou_ir_type_t;
typedef struct lou_ir_code_t lou_ir_code_t;

typedef struct lou_ir_local_t {
  lou_ir_mutability_t mutability;
  lou_ir_type_t *type;
} lou_ir_local_t;

typedef struct lou_ir_func_t {
  lou_ir_local_t **locals;
  lou_ir_code_t *code;
} lou_ir_func_t;

bool lou_ir_func_valid(lou_ir_func_t *func);
void lou_ir_func_init(lou_ir_func_t *func, lou_ir_code_t *code);
lou_ir_local_t *lou_ir_func_local_add(lou_mempool_t *mempool, lou_ir_func_t *func, lou_ir_local_t local);

static inline lou_ir_func_t *lou_ir_func_new(lou_mempool_t *mempool) {
  lou_ir_func_t *func = LOU_MEMPOOL_ALLOC(mempool, lou_ir_func_t);
  func->locals = LOU_MEMPOOL_VEC_NEW(mempool, lou_ir_local_t*);
  return func;
}
