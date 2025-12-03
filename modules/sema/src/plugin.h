#pragma once

#include "lou/core/slice.h"
#include "sema.h"

typedef struct lou_sema_type_t lou_sema_type_t;

typedef struct {
  lou_sema_t *sema;
  lou_sema_value_t **args;
  lou_slice_t *arg_slices;
  lou_slice_t slice;
} lou_sema_plugin_call_ctx_t;

typedef lou_sema_value_t *(*lou_sema_plugin_func_t)(lou_sema_plugin_call_ctx_t *ctx);

typedef struct lou_sema_plugin_t {
  lou_sema_plugin_func_t func; 
  lou_sema_type_t **args; // TODO: more flexible arg constraints
} lou_sema_plugin_t;

lou_sema_plugin_t *lou_sema_plugin_new(lou_mempool_t *mempool, lou_sema_plugin_func_t func, lou_sema_type_t **args);
bool lou_sema_plugin_ctx_check_args_count(lou_sema_plugin_call_ctx_t *ctx, size_t length);

#define LOU_SEMA_PLUGIN_EXPECT_ARG(NUMBER, CHECK, WHAT) ({ \
  typeof(CHECK(ctx->args[NUMBER])) __value = CHECK(ctx->args[NUMBER]); \
  if (!__value) { \
    lou_sema_err(ctx->sema, ctx->arg_slices[NUMBER], "expected " WHAT " got #v", ctx->args[NUMBER]); \
    return NULL; \
  }; \
  __value; \
})
