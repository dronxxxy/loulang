#include "plugin.h"
#include "lou/core/vec.h"
#include "sema.h"

bool lou_sema_plugin_ctx_check_args_count(lou_sema_plugin_call_ctx_t *ctx, size_t length) {
  if (lou_vec_len(ctx->args) != length) {
    lou_sema_err(ctx->sema, ctx->slice, "plugin expects to take #l arguments but #l were passed", length, lou_vec_len(ctx->args));
    return false;
  }
  return true;
}
