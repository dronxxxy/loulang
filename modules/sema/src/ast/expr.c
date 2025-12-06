#include "expr.h"

lou_sema_expr_ctx_t lou_sema_expr_ctx_new_runtime(lou_sema_type_t *expectation) {
  return (lou_sema_expr_ctx_t) {
    .expectation = expectation,
  };
}

lou_sema_expr_ctx_t lou_sema_expr_ctx_new_comptime() {
  return lou_sema_expr_ctx_new_runtime(NULL);
}
