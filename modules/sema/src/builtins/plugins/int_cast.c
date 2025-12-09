#include "ast/expr.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "plugin.h"
#include "type.h"
#include "value.h"

static inline lou_sema_value_t *lou_sema_builtin_int_cast_outline(lou_sema_plugin_ctx ctx, lou_sema_expr_ctx_t expr_ctx) {
  if (lou_sema_is_global_scope(ctx.sema)) {
    lou_sema_err(ctx.sema, ctx.plugin_slice, "cannot cast integers in the global scope");
    return NULL;
  }
  NOT_NULL(lou_sema_plugin_expect_n_args(&ctx, 2));
  NOT_NULL(lou_sema_expr_outline_runtime(ctx.sema, ctx.args[0], expr_ctx));
  lou_sema_type_t *type = NOT_NULL(lou_sema_expr_analyze_type(ctx.sema, ctx.args[1], expr_ctx, false));
  if (!type || type->kind != LOU_SEMA_TYPE_INTEGER) {
    lou_sema_err(ctx.sema, ctx.args[1]->slice, "an integer type was expected but got #V", type);
    return NULL;
  }
  lou_hir_local_t *local = lou_sema_add_local_final(ctx.sema, type);
  return lou_sema_value_new_local(ctx.sema->mempool, LOU_SEMA_IMMUTABLE, type, local, lou_sema_current_scope_stack(ctx.sema));
}

static inline bool lou_sema_builtin_int_cast_finalize(lou_sema_plugin_ctx ctx, lou_sema_value_t *value) {
  lou_ast_expr_t *integer_arg = ctx.args[0];
  lou_sema_value_t *integer = NOT_NULL(lou_sema_expr_finalize(ctx.sema, integer_arg, false));
  lou_sema_type_t *integer_type = lou_sema_value_is_runtime(integer);
  if (integer_type->kind != LOU_SEMA_TYPE_INTEGER) {
    lou_sema_err(ctx.sema, integer_arg->slice, "an integer was expected but got #T", integer_type);
    return false;
  }
  lou_sema_value_local_t *local = lou_sema_value_is_local(value);

  lou_sema_push_stmt(ctx.sema, ctx.plugin_slice, lou_hir_stmt_cast_int(ctx.sema->mempool, local->hir,
    lou_sema_value_as_hir(ctx.sema->mempool, integer), lou_sema_type_as_hir(ctx.sema->mempool, local->type)));
  return true;
}

lou_sema_plugin_t *lou_sema_builtin_int_cast(lou_mempool_t *mempool) {
  return lou_sema_plugin_new(mempool, lou_sema_builtin_int_cast_outline, lou_sema_builtin_int_cast_finalize);
}
