#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "mutability.h"
#include <stdbool.h>

typedef struct lou_hir_const_t lou_hir_const_t;
typedef struct lou_hir_type_t lou_hir_type_t;

typedef struct {
  lou_slice_t name;
} lou_hir_decl_extern_t;

typedef struct {
  lou_opt_slice_t name;
  lou_hir_const_t *initializer;
} lou_hir_decl_global_t;

typedef struct {
  lou_hir_const_t *initializer;
} lou_hir_decl_var_t;

typedef enum {
  LOU_HIR_DECL_EXTERN,
  LOU_HIR_DECL_GLOBAL,
} lou_hir_decl_kind_t;

typedef struct lou_hir_decl_t {
  lou_hir_mutability_t mutability;
  lou_hir_type_t *type;
  lou_hir_decl_kind_t kind;
  bool initialized;

  union {
    lou_hir_decl_extern_t external;
    lou_hir_decl_global_t global;
  };
} lou_hir_decl_t;

static inline lou_hir_decl_t *lou_hir_decl_new(lou_mempool_t *mempool, lou_hir_mutability_t mutability, lou_hir_type_t *type) {
  lou_hir_decl_t *decl = LOU_MEMPOOL_ALLOC(mempool, lou_hir_decl_t);
  decl->mutability = mutability;
  decl->type = type;
  decl->initialized = false;
  return decl;
}

void lou_hir_decl_init_extern(lou_hir_decl_t *decl, lou_hir_decl_extern_t external);
void lou_hir_decl_init_global(lou_hir_decl_t *decl, lou_hir_decl_global_t global);
