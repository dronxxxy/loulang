#pragma once

#include "lou/core/mempool.h"
#include <stdint.h>

typedef struct lou_hir_type_t lou_hir_type_t;
typedef struct lou_hir_func_t lou_hir_func_t;
typedef struct lou_hir_extern_t lou_hir_extern_t;

typedef enum {
  LOU_HIR_CONST_INTEGER,
  LOU_HIR_CONST_FUNCTION,
  LOU_HIR_CONST_EXTERNAL,
} lou_hir_const_kind_t;

typedef struct lou_hir_const_t {
  lou_hir_const_kind_t kind;
  lou_hir_type_t *type;

  union {
    struct {
      uint64_t value;
    } integer;

    lou_hir_extern_t *external;
    lou_hir_func_t *func;
  };
} lou_hir_const_t;

lou_hir_const_t *lou_hir_const_new_integer(lou_mempool_t *mempool, lou_hir_type_t *type, uint64_t value);
lou_hir_const_t *lou_hir_const_new_func(lou_mempool_t *mempool, lou_hir_func_t *func);
lou_hir_const_t *lou_hir_const_new_extern(lou_mempool_t *mempool, lou_hir_extern_t *external);
