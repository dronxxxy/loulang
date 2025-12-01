#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "mutability.h"
#include <assert.h>
#include <stdbool.h>

typedef struct lou_hir_const_t lou_hir_const_t;
typedef struct lou_hir_type_t lou_hir_type_t;

typedef struct lou_hir_decl_t {
  lou_hir_mutability_t mutability;
  lou_hir_type_t *type;
  lou_hir_const_t *initializer;

  void *codegen;
} lou_hir_decl_t;

static inline lou_hir_decl_t *lou_hir_decl_new(lou_mempool_t *mempool, lou_hir_mutability_t mutability, lou_hir_type_t *type) {
  lou_hir_decl_t *decl = LOU_MEMPOOL_ALLOC(mempool, lou_hir_decl_t);
  decl->mutability = mutability;
  decl->type = type;
  decl->initializer = NULL;
  return decl;
}

void lou_hir_decl_init(lou_hir_decl_t *decl, lou_hir_const_t *initializer);
