#pragma once

#include "lou/core/mempool.h"
typedef struct lou_ir_decl_t lou_ir_decl_t;
typedef struct lou_ir_const_t lou_ir_const_t;
typedef struct lou_ir_local_t lou_ir_local_t;

typedef enum {
  LOU_IR_VALUE_CONST,
  LOU_IR_VALUE_LOCAL,
  LOU_IR_VALUE_DECL,
} lou_ir_value_kind_t;

typedef struct lou_ir_value_t {
  lou_ir_value_kind_t kind;

  union {
    lou_ir_const_t *constant; 
    lou_ir_local_t *local; 
    lou_ir_decl_t *decl; 
  };
} lou_ir_value_t;

static inline lou_ir_value_t *lou_ir_value_new_const(lou_mempool_t *mempool, lou_ir_const_t *constant) {
  lou_ir_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_ir_value_t);
  value->kind = LOU_IR_VALUE_CONST;
  value->constant = constant;
  return value;
}

static inline lou_ir_value_t *lou_ir_value_new_local(lou_mempool_t *mempool, lou_ir_local_t *local) {
  lou_ir_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_ir_value_t);
  value->kind = LOU_IR_VALUE_LOCAL;
  value->local = local;
  return value;
}

static inline lou_ir_value_t *lou_ir_value_new_decl(lou_mempool_t *mempool, lou_ir_decl_t *decl) {
  lou_ir_value_t *value = LOU_MEMPOOL_ALLOC(mempool, lou_ir_value_t);
  value->kind = LOU_IR_VALUE_DECL;
  value->decl = decl;
  return value;
}

