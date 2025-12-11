#include "ast/expr.h"
#include "ast/expr_ctx.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "lou/hir/code.h"
#include "plugin.h"
#include "type.h"
#include "value.h"

static inline lou_sema_value_t *lou_sema_builtin_ptr_outline(lou_sema_plugin_ctx ctx, lou_sema_expr_ctx_t expr_ctx) {
  NOT_NULL(lou_sema_plugin_expect_n_args(&ctx, 1));
  return lou_sema_value_new_type(ctx.sema->mempool, lou_sema_type_new_pointer(ctx.sema->mempool));
}

static inline bool lou_sema_builtin_ptr_finalize(lou_sema_plugin_ctx ctx, lou_sema_value_t *value) {
  lou_sema_type_t *type = lou_sema_expr_analyze_type(ctx.sema, ctx.args[0], lou_sema_expr_ctx_new_comptime(), true);
  lou_sema_type_init_pointer(lou_sema_value_is_type(value), type);
  return value;
}

lou_sema_plugin_t *lou_sema_builtin_ptr(lou_mempool_t *mempool) {
  return lou_sema_plugin_new(mempool, lou_sema_builtin_ptr_outline, lou_sema_builtin_ptr_finalize);
}

static inline lou_sema_value_t *lou_sema_builtin_ptr_at_outline(lou_sema_plugin_ctx ctx, lou_sema_expr_ctx_t expr_ctx) {
  NOT_NULL(lou_sema_plugin_expect_n_args(&ctx, 2));

  lou_ast_expr_t *value_arg = ctx.args[0];
  lou_sema_value_t *value = NOT_NULL(lou_sema_expr_analyze_runtime(ctx.sema, value_arg, lou_sema_expr_ctx_nested(expr_ctx, NULL), false));
  lou_sema_type_t *type = lou_sema_value_is_runtime(value);
  if (type->kind != LOU_SEMA_TYPE_POINTER) {
    lou_sema_err(ctx.sema, value_arg->slice, "pointer was expected got value of type #T", type);
    return NULL;
  }

  lou_ast_expr_t *idx_arg = ctx.args[1];
  lou_sema_value_t *idx = NOT_NULL(lou_sema_expr_analyze_runtime(ctx.sema, idx_arg, expr_ctx, false));
  lou_sema_type_t *idx_type = lou_sema_value_is_runtime(idx);
  if (idx_type->kind != LOU_SEMA_TYPE_INTEGER) {
    lou_sema_err(ctx.sema, idx_arg->slice, "integer was expected got value of type #T", idx_type);
    return NULL;
  }

  lou_hir_local_t *local = lou_sema_add_local_pseudo_var(ctx.sema, type->pointer_to);
  lou_sema_push_stmt(ctx.sema, ctx.plugin_slice, lou_hir_stmt_new_idx_ptr(ctx.sema->mempool, local,
    lou_sema_value_as_hir(ctx.sema->mempool, value), lou_sema_value_as_hir(ctx.sema->mempool, idx)));
  return lou_sema_value_new_local(ctx.sema->mempool, LOU_SEMA_MUTABLE, type->pointer_to, local, lou_sema_current_scope_stack(ctx.sema));
}

lou_sema_plugin_t *lou_sema_builtin_ptr_at(lou_mempool_t *mempool) {
  return lou_sema_plugin_new(mempool, lou_sema_builtin_ptr_at_outline, NULL);
}
