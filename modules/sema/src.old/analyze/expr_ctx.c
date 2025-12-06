#include "expr_ctx.h"

lou_sema_expr_ctx_t lou_sema_expr_ctx_nested(lou_sema_expr_ctx_t base, lou_sema_type_t *type) {
  base.expectation = type;
  base.valuable = true;
  return base;
}

lou_sema_expr_ctx_t lou_sema_expr_ctx_new(lou_mempool_t *mempool, lou_sema_type_t *expectation, bool valuable, bool weak) {
  return (lou_sema_expr_ctx_t) {
    .expectation = expectation,
    .valuable = valuable,
    .weak = weak,
  };
}
