#include "ast/expr.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "lou/hir/func.h"
#include "plugin.h"

static inline lou_sema_value_t *lou_sema_builtin_global_outline(lou_sema_plugin_ctx ctx, lou_sema_expr_ctx_t expr_ctx) {
  NOT_NULL(lou_sema_plugin_expect_n_args(&ctx, 2));
  NOT_NULL(lou_sema_expr_outline(ctx.sema, ctx.args[0], expr_ctx));
  return NOT_NULL(lou_sema_expr_outline(ctx.sema, ctx.args[1], expr_ctx));
}

static inline bool lou_sema_builtin_global_finalize(lou_sema_plugin_ctx ctx, lou_sema_value_t *value) {
  lou_ast_expr_t *name_arg = ctx.args[0];
  lou_sema_value_t *name = NOT_NULL(lou_sema_expr_finalize(ctx.sema, name_arg, false));
  lou_sema_const_t *name_constant = lou_sema_value_is_const(name);
  if (!name_constant || name_constant->kind != LOU_SEMA_CONST_STRING) {
    lou_sema_err(ctx.sema, name_arg->slice, "a constant string was expected but got #V", name);
    return NULL;
  }

  lou_ast_expr_t *func_arg = ctx.args[1];
  lou_sema_value_t *func = NOT_NULL(lou_sema_expr_finalize(ctx.sema, func_arg, false));
  lou_sema_const_t *func_constant = lou_sema_value_is_const(func);
  if (!func_constant || func_constant->kind != LOU_SEMA_CONST_FUNC) {
    lou_sema_err(ctx.sema, func_arg->slice, "a constant function was expected but got #V", func);
    return NULL;
  }
  // TODO: check global decl duplication in project scope
  lou_hir_func_set_global(func_constant->func, name_constant->string);

  return lou_sema_value_new_type(ctx.sema->mempool, lou_sema_type_new_func(ctx.sema->mempool));
}

lou_sema_plugin_t *lou_sema_builtin_global(lou_mempool_t *mempool) {
  return lou_sema_plugin_new(mempool, lou_sema_builtin_global_outline, lou_sema_builtin_global_finalize);
}

