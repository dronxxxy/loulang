#pragma once

#include "lou/core/mempool.h"

typedef struct lou_hir_t lou_hir_t;
typedef struct lou_hir_decl_t lou_hir_decl_t;

lou_hir_t *lou_hir_new(lou_mempool_t *mempool);
void lou_hir_decl_add(lou_hir_t *ir, lou_hir_decl_t *decl);

