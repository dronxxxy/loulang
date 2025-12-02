#include "global.h"
#include "const.h"
#include "lou/core/assertions.h"
#include "lou/sema/value.h"
#include "plugin.h"
#include "sema.h"
#include "value.h"

lou_sema_value_t *lou_global_builtin(lou_sema_plugin_call_ctx_t *ctx) {
  if (!lou_sema_plugin_ctx_check_args_count(ctx, 2)) {
    return NULL;
  }
  lou_slice_t *name = lou_sema_value_is_const_string(ctx->args[0]);
  if (!name) {
    lou_sema_err(ctx->sema, ctx->arg_slices[0], "expected constant string got #v", ctx->args[0]);
    return NULL;
  }
  lou_sema_const_func_t *func = lou_sema_value_is_const_func(ctx->args[1]);
  if (!func) {
    lou_sema_err(ctx->sema, ctx->arg_slices[1], "expected constant function got #v", ctx->args[1]);
    return NULL;
  }
  func->func->name = (lou_opt_slice_t) { true, *name };
  return ctx->args[1];
}

