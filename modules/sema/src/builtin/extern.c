#include "extern.h"
#include "const.h"
#include "plugin.h"
#include "sema.h"
#include "type.h"
#include "value.h"

lou_sema_value_t *lou_extern_builtin(lou_sema_plugin_call_ctx_t *ctx) {
  if (!lou_sema_plugin_ctx_check_args_count(ctx, 2)) return NULL;

  lou_slice_t *name = LOU_SEMA_EXPECT_NOT_NULL(ctx->sema, ctx->arg_slices[0], lou_sema_value_is_const_string(ctx->args[0]), "expected constant");
  lou_sema_type_t *type = LOU_SEMA_EXPECT_NOT_NULL(ctx->sema, ctx->arg_slices[1], lou_sema_value_is_type(ctx->args[1]), "expected type");

  return lou_sema_value_new_constant(ctx->sema->mempool, lou_sema_const_new_extern(ctx->sema->mempool, lou_sema_type_new_pointer(ctx->sema->mempool, type), *name));
}

