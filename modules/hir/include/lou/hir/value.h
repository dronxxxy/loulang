#pragma once

#include "lou/core/mempool.h"
#include "lou/hir/const.h"

typedef struct lou_hir_decl_t lou_hir_decl_t;
typedef struct lou_hir_const_t lou_hir_const_t;
typedef struct lou_hir_local_t lou_hir_local_t;

typedef enum {
  LOU_HIR_VALUE_CONST,
  LOU_HIR_VALUE_LOCAL,
  LOU_HIR_VALUE_DECL,
} lou_hir_value_kind_t;

typedef struct lou_hir_value_t {
  lou_hir_value_kind_t kind;

  union {
    lou_hir_const_t *constant; 
    lou_hir_local_t *local; 
    lou_hir_decl_t *decl; 
  };
} lou_hir_value_t;

static inline lou_hir_value_t *lou_hir_value_new_const(lou_mempool_t *mempool, lou_hir_const_t *constant) {
  lou_hir_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_hir_value_t);
  value->kind = LOU_HIR_VALUE_CONST;
  value->constant = constant;
  return value;
}

static inline lou_hir_value_t *lou_hir_value_new_local(lou_mempool_t *mempool, lou_hir_local_t *local) {
  lou_hir_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_hir_value_t);
  value->kind = LOU_HIR_VALUE_LOCAL;
  value->local = local;
  return value;
}

static inline lou_hir_value_t *lou_hir_value_new_decl(lou_mempool_t *mempool, lou_hir_decl_t *decl) {
  lou_hir_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_hir_value_t);
  value->kind = LOU_HIR_VALUE_DECL;
  value->decl = decl;
  return value;
}

lou_hir_type_t *lou_hir_value_typeof(lou_hir_value_t *value);
