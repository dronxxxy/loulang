#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/hir/mutability.h"
#include <stdbool.h>

typedef struct lou_hir_type_t lou_hir_type_t;
typedef struct lou_hir_code_t lou_hir_code_t;

typedef struct lou_hir_func_t {
  lou_hir_code_t *code;
  lou_opt_slice_t name;
} lou_hir_func_t;

bool lou_hir_func_valid(lou_hir_func_t *func);
void lou_hir_func_set_global(lou_hir_func_t *func, lou_slice_t name);

lou_hir_func_t *lou_hir_func_new(lou_mempool_t *mempool);
