#pragma once

#include "lou/core/mempool.h"
#include <stdbool.h>
typedef struct lou_sema_type_t lou_sema_type_t;

typedef struct {
  lou_sema_type_t *expectation;
  bool valuable;
  bool weak;
} lou_sema_expr_ctx_t;

lou_sema_expr_ctx_t lou_sema_expr_ctx_new(lou_mempool_t *mempool, lou_sema_type_t *expectation, bool valuable, bool weak);
lou_sema_expr_ctx_t lou_sema_expr_ctx_nested(lou_sema_expr_ctx_t base, lou_sema_type_t *type);

