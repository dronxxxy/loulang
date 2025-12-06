#pragma once

#include "lou/parser/ast/expr.h"
#include "lou/sema/sema.h"
#include "ast/expr_ctx.h"

typedef struct lou_sema_value_t lou_sema_value_t;

typedef struct {
  lou_sema_t *sema;
  lou_slice_t plugin_slice;
  lou_ast_expr_t **args;
} lou_sema_plugin_ctx;

typedef lou_sema_value_t *(*lou_sema_plugin_outline_t)(lou_sema_plugin_ctx ctx, lou_sema_expr_ctx_t expr_ctx);
typedef bool (*lou_sema_plugin_finalize_t)(lou_sema_plugin_ctx ctx, lou_sema_value_t *value);

typedef struct lou_sema_plugin_t {
  lou_sema_plugin_outline_t outline;
  lou_sema_plugin_finalize_t finalize;
} lou_sema_plugin_t;

lou_sema_plugin_t *lou_sema_plugin_new(lou_mempool_t *mempool, lou_sema_plugin_outline_t outline, lou_sema_plugin_finalize_t finalize);
lou_sema_plugin_ctx lou_sema_plugin_ctx_new(lou_sema_t *sema, lou_slice_t plugin_slice, lou_ast_expr_t **exprs);

bool lou_sema_plugin_expect_n_args(const lou_sema_plugin_ctx *ctx, size_t n);
