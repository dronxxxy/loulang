#pragma once

#include "lou/core/slice.h"
#include "sema.h"

typedef struct {
  lou_sema_t *sema;
  lou_sema_value_t **args;
  lou_slice_t *arg_slices;
  lou_slice_t slice;
} lou_sema_plugin_call_ctx_t;

typedef lou_sema_value_t *(*lou_sema_plugin_func_t)(lou_sema_plugin_call_ctx_t *ctx);

typedef struct lou_sema_plugin_t {
  lou_sema_plugin_func_t func; 
} lou_sema_plugin_t;

static inline lou_sema_plugin_t *lou_sema_plugin_new(lou_mempool_t *mempool, lou_sema_plugin_func_t func) {
  lou_sema_plugin_t *plugin = LOU_MEMPOOL_ALLOC(mempool, lou_sema_plugin_t);
  plugin->func = func;
  return plugin;
}

bool lou_sema_plugin_ctx_check_args_count(lou_sema_plugin_call_ctx_t *ctx, size_t length);
