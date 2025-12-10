#include "ast/expr.h"
#include "ast/expr_ctx.h"
#include "lou/core/assertions.h"
#include "plugin.h"
#include "type.h"
#include "value.h"

static inline lou_sema_value_t *lou_sema_builtin_array_outline(lou_sema_plugin_ctx ctx, lou_sema_expr_ctx_t expr_ctx) {
  NOT_NULL(lou_sema_plugin_expect_n_args(&ctx, 2));

  lou_ast_expr_t *length_arg = ctx.args[0];
  lou_sema_value_t *length = NOT_NULL(lou_sema_expr_analyze(ctx.sema, length_arg, expr_ctx, false));
  lou_sema_const_t *length_constant = lou_sema_value_is_const(length);
  if (!length_constant || length_constant->kind != LOU_SEMA_CONST_INTEGER) {
    lou_sema_err(ctx.sema, length_arg->slice, "a constant integer was expected but got #V", length);
    return NULL;
  }

  lou_ast_expr_t *type_arg = ctx.args[1];
  lou_sema_type_t *type = NOT_NULL(lou_sema_expr_analyze_type(ctx.sema, type_arg, lou_sema_expr_ctx_nested(expr_ctx, NULL), false));

  return lou_sema_value_new_type(ctx.sema->mempool, lou_sema_type_new_array(ctx.sema->mempool, type, length_constant->integer));
}

static inline bool lou_sema_builtin_array_finalize(lou_sema_plugin_ctx ctx, lou_sema_value_t *value) {
  return true;
}

lou_sema_plugin_t *lou_sema_builtin_array(lou_mempool_t *mempool) {
  return lou_sema_plugin_new(mempool, lou_sema_builtin_array_outline, lou_sema_builtin_array_finalize);
}


