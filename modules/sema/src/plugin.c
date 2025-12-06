#include "plugin.h"
#include "lou/core/mempool.h"

lou_sema_plugin_t *lou_sema_plugin_new(lou_mempool_t *mempool, lou_sema_plugin_outline_t outline, lou_sema_plugin_finalize_t finalize) {
  lou_sema_plugin_t *plugin = LOU_MEMPOOL_ALLOC(mempool, lou_sema_plugin_t);
  plugin->outline = outline;
  plugin->finalize = finalize;
  return plugin;
}

lou_sema_plugin_ctx lou_sema_plugin_ctx_new(lou_sema_t *sema, lou_slice_t plugin_slice, lou_ast_expr_t **exprs) {
  return (lou_sema_plugin_ctx) {
    .sema = sema,
    .plugin_slice = plugin_slice,
    .exprs = exprs,
  };
}
