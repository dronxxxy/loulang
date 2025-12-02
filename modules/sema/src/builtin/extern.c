#include "extern.h"
#include "const.h"
#include "lou/sema/value.h"
#include "plugin.h"
#include "sema.h"
#include "value.h"

lou_sema_value_t *lou_extern_builtin(lou_sema_plugin_call_ctx_t *ctx) {
  if (!lou_sema_plugin_ctx_check_args_count(ctx, 2)) return NULL;
  lou_slice_t *name = LOU_SEMA_PLUGIN_EXPECT_ARG(0, lou_sema_value_is_const_string, "constant string");
  lou_sema_type_t *type = LOU_SEMA_PLUGIN_EXPECT_ARG(1, lou_sema_value_is_type, "type");
  (void)name;
  (void)type;

  lou_sema_err(ctx->sema, ctx->slice, "extern is NIY");
  return ctx->args[1];
}

