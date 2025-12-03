#include "const.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "plugin.h"
#include "sema.h"
#include "type.h"
#include "value.h"

lou_sema_value_t *lou_fun_builtin(lou_sema_plugin_call_ctx_t *ctx) {
  if (!lou_sema_plugin_ctx_check_args_count(ctx, 1)) return NULL;
  lou_sema_type_t *type = LOU_SEMA_PLUGIN_EXPECT_ARG(0, lou_sema_value_is_type, "type");

  lou_sema_type_func_arg_t *args = LOU_MEMPOOL_VEC_NEW(ctx->sema->mempool, lou_sema_type_func_arg_t);
  *LOU_VEC_PUSH(&args) = (lou_sema_type_func_arg_t) {
    .name = { false },
    .type = type,
  };

  return lou_sema_value_new_type(ctx->sema->mempool, lou_sema_type_new_function(ctx->sema->mempool, args, NULL));
}

lou_sema_value_t *lou_ret_fun_builtin(lou_sema_plugin_call_ctx_t *ctx) {
  if (!lou_sema_plugin_ctx_check_args_count(ctx, 1)) return NULL;
  lou_sema_type_t *type = LOU_SEMA_PLUGIN_EXPECT_ARG(0, lou_sema_value_is_type, "type");

  return lou_sema_value_new_type(ctx->sema->mempool, lou_sema_type_new_function(ctx->sema->mempool, LOU_MEMPOOL_VEC_NEW(ctx->sema->mempool, lou_sema_type_func_arg_t), type));
}

