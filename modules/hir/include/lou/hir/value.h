#pragma once

#include "lou/core/mempool.h"
#include "lou/hir/const.h"
#include "lou/hir/mutability.h"
#include <stddef.h>

typedef struct lou_hir_const_t lou_hir_const_t;
typedef struct lou_hir_local_t lou_hir_local_t;

typedef enum {
  LOU_HIR_VALUE_CONST,
  LOU_HIR_VALUE_LOCAL,
} lou_hir_value_kind_t;

typedef struct lou_hir_value_t {
  lou_hir_value_kind_t kind;

  union {
    lou_hir_const_t *constant; 
    lou_hir_local_t *local; 
  };
} lou_hir_value_t;

lou_hir_value_t *lou_hir_value_new_const(lou_mempool_t *mempool, lou_hir_const_t *constant);
lou_hir_value_t *lou_hir_value_new_local(lou_mempool_t *mempool, lou_hir_local_t *local);

lou_hir_type_t *lou_hir_value_typeof(lou_hir_value_t *value);
lou_hir_mutability_t lou_hir_value_mutability(lou_hir_value_t *value);
