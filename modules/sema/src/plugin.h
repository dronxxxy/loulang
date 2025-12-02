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

typedef enum {
  LOU_SEMA_PLUGIN_CONSTRAINT_NONE,
  LOU_SEMA_PLUGIN_CONSTRAINT_TYPE,
  LOU_SEMA_PLUGIN_CONSTRAINT_CONST,
  LOU_SEMA_PLUGIN_CONSTRAINT_GLOBAL_DECL,
} lou_sema_plugin_constraint_kind_t;

typedef struct {
  lou_sema_type_t *type;
} lou_sema_plugin_val_constraint_t;

typedef struct {
  lou_sema_plugin_constraint_kind_t kind;

  union {
    lou_sema_type_t *type;
    lou_sema_plugin_val_constraint_t constant;
    lou_sema_plugin_val_constraint_t global_decl;
  };
} lou_sema_plugin_constraint_t;

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

#define LOU_SEMA_PLUGIN_EXPECT_ARG(NUMBER, CHECK, WHAT) ({ \
  typeof(CHECK(ctx->args[NUMBER])) __value = CHECK(ctx->args[NUMBER]); \
  if (!__value) { \
    lou_sema_err(ctx->sema, ctx->arg_slices[NUMBER], "expected " WHAT " got #v", ctx->args[NUMBER]); \
    return NULL; \
  }; \
  __value; \
})
