#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "mutability.h"
#include <stdbool.h>

typedef struct lou_ir_const_t lou_ir_const_t;
typedef struct lou_ir_type_t lou_ir_type_t;

typedef struct {
  lou_slice_t name;
} lou_ir_decl_extern_t;

typedef struct {
  lou_opt_slice_t name;
  lou_ir_const_t *initializer;
} lou_ir_decl_global_t;

typedef struct {
  lou_ir_const_t *initializer;
} lou_ir_decl_var_t;

typedef enum {
  LOU_IR_DECL_EXTERN,
  LOU_IR_DECL_GLOBAL,
} lou_ir_decl_kind_t;

typedef struct lou_ir_decl_t {
  lou_ir_mutability_t mutability;
  lou_ir_type_t *type;
  lou_ir_decl_kind_t kind;
  bool initialized;

  union {
    lou_ir_decl_extern_t external;
    lou_ir_decl_global_t global;
  };
} lou_ir_decl_t;

static inline lou_ir_decl_t *lou_ir_decl_new(lou_mempool_t *mempool, lou_ir_mutability_t mutability, lou_ir_type_t *type) {
  lou_ir_decl_t *decl = LOU_MEMPOOL_ALLOC(mempool, lou_ir_decl_t);
  decl->mutability = mutability;
  decl->type = type;
  decl->initialized = false;
  return decl;
}

void lou_ir_decl_init_extern(lou_ir_decl_t *decl, lou_ir_decl_extern_t external);
void lou_ir_decl_init_global(lou_ir_decl_t *decl, lou_ir_decl_global_t global);
