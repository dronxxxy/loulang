#pragma once

#include "lou/core/mempool.h"
#include <stdint.h>

typedef struct lou_hir_type_t lou_hir_type_t;
typedef struct lou_hir_code_t lou_hir_code_t;
typedef struct lou_hir_func_t lou_hir_func_t;

typedef struct {
  uint64_t value;
} lou_hir_const_int_t;

typedef struct {
  lou_slice_t name;
} lou_hir_const_extern_t;

typedef enum {
  LOU_HIR_CONST_INTEGER,
  LOU_HIR_CONST_FUNCTION,
  LOU_HIR_CONST_EXTERNAL,
} lou_hir_const_kind_t;

typedef struct lou_hir_const_t {
  lou_hir_const_kind_t kind;
  lou_hir_type_t *type;

  union {
    lou_hir_const_int_t integer;
    lou_hir_const_extern_t external;
    lou_hir_func_t *func;
  };
} lou_hir_const_t;

static inline lou_hir_const_t *lou_hir_const_new_integer(lou_mempool_t *mempool, lou_hir_type_t *type, lou_hir_const_int_t integer) {
  lou_hir_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_hir_const_t);
  constant->kind = LOU_HIR_CONST_INTEGER;
  constant->type = type;
  constant->integer = integer;
  return constant;
}

static inline lou_hir_const_t *lou_hir_const_new_func(lou_mempool_t *mempool, lou_hir_type_t *type, lou_hir_func_t *func) {
  lou_hir_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_hir_const_t);
  constant->kind = LOU_HIR_CONST_FUNCTION;
  constant->type = type;
  constant->func = func;
  return constant;
}


static inline lou_hir_const_t *lou_hir_const_new_extern(lou_mempool_t *mempool, lou_hir_type_t *type, lou_hir_const_extern_t external) {
  lou_hir_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_hir_const_t);
  constant->kind = LOU_HIR_CONST_EXTERNAL;
  constant->type = type;
  constant->external = external;
  return constant;
}
