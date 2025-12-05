#pragma once

#include "lou/core/slice.h"
#include <stdbool.h>

typedef struct lou_hir_type_t lou_hir_type_t;
typedef struct lou_hir_code_t lou_hir_code_t;

typedef struct lou_hir_func_t {
  lou_hir_type_t *type;
  lou_hir_code_t *code;
  lou_opt_slice_t name;

  void *codegen;
} lou_hir_func_t;

void lou_hir_func_set_global(lou_hir_func_t *func, lou_slice_t name);
void lou_hir_func_init(lou_hir_func_t *func, lou_hir_code_t *code);
