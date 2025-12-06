#include "extern.h"
#include "analyze/value.h"
#include "const.h"
#include "lou/hir/hir.h"
#include "plugin.h"
#include "impl.h"
#include "type.h"
#include "value.h"

lou_sema_value_t *lou_extern_fun_builtin(lou_sema_plugin_call_ctx_t *ctx) {
  if (!lou_sema_plugin_ctx_check_args_count(ctx, 2)) return NULL;

  lou_slice_t *name = LOU_SEMA_EXPECT_NOT_NULL(ctx->sema, ctx->arg_slices[0], lou_sema_value_is_const_string(ctx->args[0]), "expected constant");
  lou_sema_type_t *type = LOU_SEMA_EXPECT_NOT_NULL(ctx->sema, ctx->arg_slices[1], lou_sema_value_is_type(ctx->args[1]), "expected type");
  if (type->kind != LOU_SEMA_TYPE_FUNCTION) {
    lou_sema_err(ctx->sema, ctx->arg_slices[1], "expected function type got #T", type);
    return NULL;
  }

  lou_hir_extern_t *external = lou_hir_extern_add(ctx->sema->hir, lou_sema_emit_type(ctx->sema->mempool, type), *name);
  return lou_sema_value_new_constant(ctx->sema->mempool, lou_sema_const_new_extern(ctx->sema->mempool, type, external));
}

lou_sema_value_t *lou_extern_var_builtin(lou_sema_plugin_call_ctx_t *ctx) {
  if (!lou_sema_plugin_ctx_check_args_count(ctx, 2)) return NULL;

  lou_slice_t *name = LOU_SEMA_EXPECT_NOT_NULL(ctx->sema, ctx->arg_slices[0], lou_sema_value_is_const_string(ctx->args[0]), "expected constant");
  lou_sema_type_t *type = LOU_SEMA_EXPECT_NOT_NULL(ctx->sema, ctx->arg_slices[1], lou_sema_value_is_type(ctx->args[1]), "expected type");

  lou_hir_extern_t *external = lou_hir_extern_add(ctx->sema->hir, lou_sema_emit_type(ctx->sema->mempool, type), *name);
  return lou_sema_value_new_constant(ctx->sema->mempool, lou_sema_const_new_extern(ctx->sema->mempool, lou_sema_type_new_pointer(ctx->sema->mempool, type), external));
}
