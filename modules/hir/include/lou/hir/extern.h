#pragma once
#include "lou/core/slice.h"

typedef struct lou_hir_type_t lou_hir_type_t;

typedef struct lou_hir_extern_t {
  lou_hir_type_t *type;
  lou_slice_t name;

  void *codegen;
} lou_hir_extern_t;

