#include "ast/expr.h"
#include "ast/expr_ctx.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "lou/hir/hir.h"
#include "plugin.h"
#include "type.h"
#include "value.h"

static inline lou_sema_value_t *lou_sema_builtin_extern_fun_outline(lou_sema_plugin_ctx ctx, lou_sema_expr_ctx_t expr_ctx) {
  NOT_NULL(lou_sema_plugin_expect_n_args(&ctx, 2));
  lou_ast_expr_t *name_arg = ctx.args[0];
  lou_sema_value_t *name = NOT_NULL(lou_sema_expr_analyze(ctx.sema, name_arg, expr_ctx, false));
  lou_sema_const_t *name_constant = lou_sema_value_is_const(name);
  if (!name_constant || name_constant->kind != LOU_SEMA_CONST_STRING) {
    lou_sema_err(ctx.sema, name_arg->slice, "a constant string was expected but got #V", name);
    return NULL;
  }

  lou_ast_expr_t *type_arg = ctx.args[1];
  lou_sema_type_t *type = NOT_NULL(lou_sema_expr_analyze_type(ctx.sema, type_arg, lou_sema_expr_ctx_nested(expr_ctx, NULL), false));
  if (type->kind != LOU_SEMA_TYPE_FUNCTION) {
    lou_sema_err(ctx.sema, name_arg->slice, "a function type was expected but got #T", type);
    return NULL;
  }

  lou_hir_extern_t *external = lou_hir_extern_add(ctx.sema->hir, lou_sema_type_as_hir(ctx.sema->mempool, type), name_constant->string);
  lou_sema_const_t *constant = lou_sema_const_new_extern(ctx.sema->mempool, type, external);
  return lou_sema_value_new_const(ctx.sema->mempool, constant);
}

static inline bool lou_sema_builtin_extern_fun_finalize(lou_sema_plugin_ctx ctx, lou_sema_value_t *value) {
  return true;
}

lou_sema_plugin_t *lou_sema_builtin_extern_fun(lou_mempool_t *mempool) {
  return lou_sema_plugin_new(mempool, lou_sema_builtin_extern_fun_outline, lou_sema_builtin_extern_fun_finalize);
}

