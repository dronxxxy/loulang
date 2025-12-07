#include "ast/expr.h"
#include "ast/expr_ctx.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "plugin.h"
#include "type.h"
#include "value.h"

static inline lou_sema_value_t *lou_sema_builtin_ret_fun_outline(lou_sema_plugin_ctx ctx, lou_sema_expr_ctx_t expr_ctx) {
  NOT_NULL(lou_sema_plugin_expect_n_args(&ctx, 1));
  return lou_sema_value_new_type(ctx.sema->mempool, lou_sema_type_new_func(ctx.sema->mempool));
}

static inline bool lou_sema_builtin_ret_fun_finalize(lou_sema_plugin_ctx ctx, lou_sema_value_t *value) {
  lou_sema_type_t *type = lou_sema_value_is_type(value);
  assert(type);

  lou_sema_type_t *arg_type = NOT_NULL(lou_sema_expr_analyze_type(ctx.sema, ctx.args[0], lou_sema_expr_ctx_new_comptime(), true));

  lou_sema_type_init_func(type, LOU_MEMPOOL_VEC_NEW(ctx.sema->mempool, lou_sema_type_t*), arg_type);
  return true;
}

lou_sema_plugin_t *lou_sema_builtin_ret_fun(lou_mempool_t *mempool) {
  return lou_sema_plugin_new(mempool, lou_sema_builtin_ret_fun_outline, lou_sema_builtin_ret_fun_finalize);
}

static inline lou_sema_value_t *lou_sema_builtin_proc_fun_outline(lou_sema_plugin_ctx ctx, lou_sema_expr_ctx_t expr_ctx) {
  return lou_sema_value_new_type(ctx.sema->mempool, lou_sema_type_new_func(ctx.sema->mempool));
}

static inline bool lou_sema_builtin_proc_fun_finalize(lou_sema_plugin_ctx ctx, lou_sema_value_t *value) {
  lou_sema_type_t *type = lou_sema_value_is_type(value);
  assert(type);

  lou_sema_type_t **args = LOU_MEMPOOL_VEC_NEW(ctx.sema->mempool, lou_sema_type_t*);
  for (size_t i = 0; i < lou_vec_len(ctx.args); i++) {
    *LOU_VEC_PUSH(&args) = NOT_NULL(lou_sema_expr_analyze_type(ctx.sema, ctx.args[0], lou_sema_expr_ctx_new_comptime(), true));
  }

  lou_sema_type_init_func(type, args, NULL);
  return true;
}

lou_sema_plugin_t *lou_sema_builtin_proc_fun(lou_mempool_t *mempool) {
  return lou_sema_plugin_new(mempool, lou_sema_builtin_proc_fun_outline, lou_sema_builtin_proc_fun_finalize);
}

