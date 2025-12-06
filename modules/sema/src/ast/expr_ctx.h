#pragma once

#include "type.h"

typedef struct {
  lou_sema_type_t *expectation;
} lou_sema_expr_ctx_t;

lou_sema_expr_ctx_t lou_sema_expr_ctx_new_runtime(lou_sema_type_t *expectation);
lou_sema_expr_ctx_t lou_sema_expr_ctx_new_comptime();
lou_sema_expr_ctx_t lou_sema_expr_ctx_nested(lou_sema_expr_ctx_t ctx, lou_sema_type_t *expectation);

