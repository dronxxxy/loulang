#pragma once

#include "lou/parser/ast/expr.h"
#include "lou/sema/sema.h"
#include "value.h"

typedef struct lou_sema_type_t lou_sema_type_t;

typedef struct {
  lou_sema_type_t *expectation;
} lou_sema_expr_ctx_t;

lou_sema_value_t *lou_sema_analyze_expr(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx);
lou_sema_type_t *lou_sema_analyze_type(lou_sema_t *sema, lou_ast_expr_t *expr);

static inline lou_sema_expr_ctx_t lou_sema_expr_ctx_new(lou_sema_type_t *expectation) {
  return (lou_sema_expr_ctx_t) {
    .expectation = expectation,
  };
}

lou_sema_expr_ctx_t lou_sema_expr_ctx_nested(lou_sema_expr_ctx_t base, lou_sema_type_t *type);
