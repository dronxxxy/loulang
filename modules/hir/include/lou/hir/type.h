#pragma once

#include "lou/core/mempool.h"
#include <stdbool.h>

typedef struct lou_hir_type_t lou_hir_type_t;

typedef enum {
  LOU_HIR_INT_8,
  LOU_HIR_INT_16,
  LOU_HIR_INT_32,
  LOU_HIR_INT_64,
} lou_hir_int_size_t;

typedef struct {
  lou_hir_int_size_t size;
  bool is_signed;
} lou_hir_type_int_t;

typedef struct {
  lou_hir_type_t **args;
  lou_hir_type_t *returns;
} lou_hir_type_func_t;

typedef enum {
  LOU_HIR_TYPE_INT,
  LOU_HIR_TYPE_FUNC,
  LOU_HIR_TYPE_POINTER,
  LOU_HIR_TYPE_BOOL,
} lou_hir_type_kind_t;

typedef struct lou_hir_type_t {
  lou_hir_type_kind_t kind;

  union {
    lou_hir_type_int_t integer;
    lou_hir_type_func_t func;
    lou_hir_type_t *pointer_to;
  };
} lou_hir_type_t;

lou_hir_type_t *lou_hir_type_new_pointer(lou_mempool_t *mempool, lou_hir_type_t *to);
lou_hir_type_t *lou_hir_type_new_func(lou_mempool_t *mempool, lou_hir_type_t **args, lou_hir_type_t *returns);
lou_hir_type_t *lou_hir_type_new_integer(lou_mempool_t *mempool, lou_hir_int_size_t size, bool is_signed);
lou_hir_type_t *lou_hir_type_new_bool(lou_mempool_t *mempool);
