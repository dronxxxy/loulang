#include "const.h"
#include "plugin.h"
#include "impl.h"
#include "type.h"
#include "value.h"

lou_sema_value_t *lou_ptr_builtin(lou_sema_plugin_call_ctx_t *ctx) {
  if (!lou_sema_plugin_ctx_check_args_count(ctx, 1)) return NULL;
  lou_sema_type_t *type = LOU_SEMA_PLUGIN_EXPECT_ARG(0, lou_sema_value_is_type, "type");
  return lou_sema_value_new_type(ctx->sema->mempool, lou_sema_type_new_pointer(ctx->sema->mempool, type));
}

