#include "ast/expr.h"
#include "ast/expr_ctx.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "plugin.h"
#include "type.h"
#include "value.h"
#include <stdio.h>

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

lou_sema_plugin_t *lou_sema_builtin_array(lou_mempool_t *mempool) {
  return lou_sema_plugin_new(mempool, lou_sema_builtin_array_outline, NULL);
}

static inline lou_sema_value_t *lou_sema_builtin_array_at_outline(lou_sema_plugin_ctx ctx, lou_sema_expr_ctx_t expr_ctx) {
  NOT_NULL(lou_sema_plugin_expect_n_args(&ctx, 2));

  lou_ast_expr_t *value_arg = ctx.args[0];
  lou_sema_value_t *value = NOT_NULL(lou_sema_expr_analyze_runtime(ctx.sema, value_arg, lou_sema_expr_ctx_nested(expr_ctx, NULL), false));
  lou_sema_type_t *type = lou_sema_value_is_runtime(value);
  if (type->kind != LOU_SEMA_TYPE_ARRAY) {
    lou_sema_err(ctx.sema, value_arg->slice, "array was expected got value of type #T", type);
    return NULL;
  }

  lou_ast_expr_t *idx_arg = ctx.args[1];
  lou_sema_value_t *idx = NOT_NULL(lou_sema_expr_analyze_runtime(ctx.sema, idx_arg, expr_ctx, false));
  lou_sema_type_t *idx_type = lou_sema_value_is_runtime(idx);
  if (idx_type->kind != LOU_SEMA_TYPE_INTEGER) {
    lou_sema_err(ctx.sema, idx_arg->slice, "integer was expected got value of type #T", idx_type);
    return NULL;
  }

  lou_sema_type_t *element_type = type->array.of;
  lou_sema_value_local_t *local = lou_sema_value_is_local(value);
  if (local && local->mutability == LOU_SEMA_MUTABLE) {
    lou_hir_local_t *output = lou_sema_add_local_pseudo_var(ctx.sema, type);
    lou_sema_push_stmt(ctx.sema, ctx.plugin_slice, lou_hir_stmt_new_idx_var_array(ctx.sema->mempool, output,
      local->hir, lou_sema_value_as_hir(ctx.sema->mempool, idx)));
    return lou_sema_value_new_local(ctx.sema->mempool, LOU_SEMA_MUTABLE, element_type, output, lou_sema_current_scope_stack(ctx.sema));
  } else {
    lou_hir_local_t *local = lou_sema_add_local_final(ctx.sema, type);
    lou_sema_push_stmt(ctx.sema, ctx.plugin_slice, lou_hir_stmt_new_idx_array(ctx.sema->mempool, local,
      lou_sema_value_as_hir(ctx.sema->mempool, value), lou_sema_value_as_hir(ctx.sema->mempool, idx)));
    return lou_sema_value_new_local(ctx.sema->mempool, LOU_SEMA_IMMUTABLE, element_type, local, lou_sema_current_scope_stack(ctx.sema));
  }
}

lou_sema_plugin_t *lou_sema_builtin_array_at(lou_mempool_t *mempool) {
  return lou_sema_plugin_new(mempool, lou_sema_builtin_array_at_outline, NULL);
}

