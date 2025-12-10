#include "ast/expr.h"
#include "ast/expr_ctx.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "lou/hir/code.h"
#include "plugin.h"
#include "value.h"

static inline lou_sema_value_t *lou_sema_builtin_deref_outline(lou_sema_plugin_ctx ctx, lou_sema_expr_ctx_t expr_ctx) {
  NOT_NULL(lou_sema_plugin_expect_n_args(&ctx, 1));

  lou_ast_expr_t *value_arg = ctx.args[0];
  lou_sema_value_t *value = NOT_NULL(lou_sema_expr_analyze_runtime(ctx.sema, value_arg, expr_ctx, false));
  lou_sema_type_t *type = lou_sema_value_is_runtime(value);
  if (type->kind != LOU_SEMA_TYPE_POINTER) {
    lou_sema_err(ctx.sema, value_arg->slice, "only pointers can be dereferenced but value has type #T", type);
    return NULL;
  }

  lou_hir_local_t *local = lou_sema_add_local_pseudo_var(ctx.sema, type);
  lou_sema_push_stmt(ctx.sema, ctx.plugin_slice, lou_hir_stmt_new_set_pseudo_var(ctx.sema->mempool, local,
    lou_sema_value_as_hir(ctx.sema->mempool, value)));
  return lou_sema_value_new_local(ctx.sema->mempool, LOU_SEMA_MUTABLE, type, local, lou_sema_current_scope_stack(ctx.sema));
}

lou_sema_plugin_t *lou_sema_builtin_deref(lou_mempool_t *mempool) {
  return lou_sema_plugin_new(mempool, lou_sema_builtin_deref_outline, NULL);
}


