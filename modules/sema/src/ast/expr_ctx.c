#include "expr_ctx.h"

lou_sema_expr_ctx_t lou_sema_expr_ctx_new_runtime(lou_sema_type_t *expectation) {
  return (lou_sema_expr_ctx_t) {
    .expectation = expectation,
  };
}

lou_sema_expr_ctx_t lou_sema_expr_ctx_new_comptime() {
  return lou_sema_expr_ctx_new_runtime(NULL);
}

lou_sema_expr_ctx_t lou_sema_expr_ctx_nested(lou_sema_expr_ctx_t ctx, lou_sema_type_t *expectation) {
  ctx.expectation = expectation;
  return ctx;
}
