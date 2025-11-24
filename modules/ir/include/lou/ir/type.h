#pragma once

#include "lou/core/mempool.h"
#include <stdbool.h>

typedef struct lou_ir_type_t lou_ir_type_t;

typedef enum {
  LOU_IR_INT_8,
  LOU_IR_INT_16,
  LOU_IR_INT_32,
  LOU_IR_INT_64,
} lou_ir_int_size_t;

typedef struct {
  lou_ir_int_size_t size;
  bool is_signed;
} lou_ir_type_int_t;

typedef struct {
  lou_ir_type_t **args;
  lou_ir_type_t *returns;
} lou_ir_type_func_t;

typedef enum {
  LOU_IR_TYPE_INT,
  LOU_IR_TYPE_FUNC,
} lou_ir_type_kind_t;

typedef struct lou_ir_type_t {
  lou_ir_type_kind_t kind;

  union {
    lou_ir_type_int_t integer;
    lou_ir_type_func_t func;
  };
} lou_ir_type_t;

static inline lou_ir_type_t *lou_ir_type_new_func(lou_mempool_t *mempool, lou_ir_type_func_t func)  {
  lou_ir_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_ir_type_t);
  type->kind = LOU_IR_TYPE_FUNC;
  type->func = func;
  return type;
}

static inline lou_ir_type_t *lou_ir_type_new_integer(lou_mempool_t *mempool, lou_ir_type_int_t integer)  {
  lou_ir_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_ir_type_t);
  type->kind = LOU_IR_TYPE_INT;
  type->integer = integer;
  return type;
}
