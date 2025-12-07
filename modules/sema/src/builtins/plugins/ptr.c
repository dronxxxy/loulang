#include "ast/expr.h"
#include "ast/expr_ctx.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "lou/hir/hir.h"
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

