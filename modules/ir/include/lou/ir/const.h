#pragma once

#include "lou/core/mempool.h"
#include <stdint.h>

typedef struct lou_ir_type_t lou_ir_type_t;
typedef struct lou_ir_code_t lou_ir_code_t;
typedef struct lou_ir_func_t lou_ir_func_t;

typedef struct {
  uint64_t value;
} lou_ir_const_int_t;

typedef enum {
  LOU_IR_CONST_INTEGER,
  LOU_IR_CONST_FUNCTION,
} lou_ir_const_kind_t;

typedef struct lou_ir_const_t {
  lou_ir_const_kind_t kind;
  lou_ir_type_t *type;

  union {
    lou_ir_const_int_t integer;  
    lou_ir_func_t *func;
  };
} lou_ir_const_t;

static inline lou_ir_const_t *lou_ir_const_new_integer(lou_mempool_t *mempool, lou_ir_type_t *type, lou_ir_const_int_t integer) {
  lou_ir_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_ir_const_t);
  constant->kind = LOU_IR_CONST_INTEGER;
  constant->type = type;
  constant->integer = integer;
  return constant;
}

static inline lou_ir_const_t *lou_ir_const_new_func(lou_mempool_t *mempool, lou_ir_type_t *type, lou_ir_func_t *func) {
  lou_ir_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_ir_const_t);
  constant->kind = LOU_IR_CONST_FUNCTION;
  constant->type = type;
  constant->func = func;
  return constant;
}
