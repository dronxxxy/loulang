#include "impl.h"
#include "lou/core/assertions.h"
#include "plugin.h"

static inline lou_sema_value_t *lou_sema_builtin_extern_fun_outline(lou_sema_plugin_ctx ctx, lou_sema_expr_ctx_t expr_ctx) {
  lou_sema_err(ctx.sema, ctx.plugin_slice, "plugin is not implemented yet");
  return NULL;
}

static inline bool lou_sema_builtin_extern_fun_finalize(lou_sema_plugin_ctx ctx, lou_sema_value_t *value) {
  NOT_IMPLEMENTED;
}

lou_sema_plugin_t *lou_sema_builtin_extern_fun(lou_mempool_t *mempool) {
  return lou_sema_plugin_new(mempool, lou_sema_builtin_extern_fun_outline, lou_sema_builtin_extern_fun_finalize);
}

