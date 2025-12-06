#include "global.h"
#include "const.h"
#include "plugin.h"
#include "impl.h"
#include "value.h"

lou_sema_value_t *lou_global_builtin(lou_sema_plugin_call_ctx_t *ctx) {
  if (!lou_sema_plugin_ctx_check_args_count(ctx, 2)) return NULL;
  lou_slice_t *name = LOU_SEMA_PLUGIN_EXPECT_ARG(0, lou_sema_value_is_const_string, "constant string");
  lou_sema_const_t *constant = LOU_SEMA_PLUGIN_EXPECT_ARG(1, lou_sema_value_is_const_func, "constant function");

  lou_hir_func_set_global(constant->func, *name);
  return ctx->args[1];
}

