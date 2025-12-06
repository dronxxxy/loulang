#include "expr.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "value.h"

lou_sema_value_t *lou_sema_expr_outline(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  NOT_IMPLEMENTED;
}

bool lou_sema_expr_finalize(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_value_t *value) {
  NOT_IMPLEMENTED;
}

lou_sema_value_t *lou_sema_expr_outline_runtime(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_value_t *value = NOT_NULL(lou_sema_expr_outline(sema, expr, ctx));
  if (!lou_sema_value_is_runtime(value)) {
    lou_sema_err(sema, expr->slice, "a runtime expression was expected but got #V", value);
  }
  return value;
}

lou_sema_value_t *lou_sema_expr_analyze(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_value_t *value = NOT_NULL(lou_sema_expr_outline(sema, expr, ctx));
  if (!lou_sema_expr_finalize(sema, expr, value)) return NULL;
  return value;
}

lou_sema_type_t *lou_sema_expr_analyze_type(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_value_t *value = NOT_NULL(lou_sema_expr_outline(sema, expr, ctx));
  lou_sema_type_t *type = lou_sema_value_is_type(value);
  if (!type) {
    lou_sema_err(sema, expr->slice, "a type was expected but got #V", value);
  }
  return type;
}

lou_sema_expr_ctx_t lou_sema_expr_ctx_new_runtime(lou_sema_type_t *expectation) {
  return (lou_sema_expr_ctx_t) {
    .expectation = expectation,
  };
}

lou_sema_expr_ctx_t lou_sema_expr_ctx_new_comptime() {
  return lou_sema_expr_ctx_new_runtime(NULL);
}
