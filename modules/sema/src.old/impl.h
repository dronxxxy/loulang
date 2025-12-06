#pragma once

#include "analyze/expr.h"
#include "lou/core/mempool.h"
#include "lou/hir/code.h"
#include "lou/hir/hir.h"
#include "lou/parser/ast/node.h"
#include "lou/parser/parser.h"
#include "scope.h"
#include "value.h"
#include <stdbool.h>

typedef enum {
  LOU_SEMA_DECL_META,
  LOU_SEMA_DECL_CONSTANT,
  LOU_SEMA_DECL_FINAL,
  LOU_SEMA_DECL_VARIABLE,
} lou_sema_decl_kind_t;

typedef struct lou_sema_t lou_sema_t;
typedef struct lou_sema_plugin_t lou_sema_plugin_t;
typedef struct lou_sema_value_t lou_sema_value_t;

typedef struct lou_sema_decl_t {
  lou_sema_t *in;
  bool is_public;
  lou_slice_t name;
  lou_sema_type_t *type;
  lou_sema_decl_kind_t kind;

  lou_ast_node_t *prefetch_node;
  lou_sema_value_t *value;
  lou_sema_scope_frame_t *scope_frame;
} lou_sema_decl_t;

typedef struct {
  lou_ast_node_t *node;
  lou_ast_expr_t *initializer;
  lou_sema_value_t *prefetched_value;
} lou_sema_analysing_node_t;

typedef struct lou_sema_t {
  lou_parser_t *parser;
  lou_mempool_t *mempool;
  lou_ast_node_t **nodes;
  bool failed;
  
  lou_hir_t *hir;
  lou_sema_analysing_node_t *node_stack;
  lou_sema_decl_t **global_decls;
  lou_sema_scope_frame_t **scope_frames;
} lou_sema_t;

void lou_sema_init_decl(lou_sema_t *sema, lou_slice_t slice, lou_sema_decl_t *decl, lou_sema_value_t *value);

bool lou_sema_is_initializer_expr(const lou_sema_t *sema, const lou_ast_expr_t *expr);
bool lou_sema_is_analysing_node(const lou_sema_t *sema, const lou_ast_node_t *node);
void lou_sema_push_analysing_node(lou_sema_t *sema, lou_ast_node_t *node, lou_ast_expr_t *initializer);
void lou_sema_prefetch_current_node(lou_sema_t *sema, lou_sema_value_t *value);
void lou_sema_pop_analysing_node(lou_sema_t *sema);

lou_sema_decl_t *lou_sema_add_decl(
  lou_sema_t *sema,
  bool is_public,
  lou_slice_t name,
  lou_sema_decl_kind_t kind,
  lou_ast_node_t *prefetch_node
);

void lou_sema_err(lou_sema_t *sema, lou_slice_t slice, const char *fmt, ...);
lou_sema_value_t *lou_sema_resolve(lou_sema_t *sema, lou_slice_t name, bool weak);
lou_sema_value_t *lou_sema_call_plugin(
  lou_sema_t *sema,
  lou_sema_plugin_t *plugin,
  lou_slice_t slice,
  lou_slice_t *arg_slices,
  lou_sema_value_t **args,
  lou_ast_expr_t *expr,
  lou_sema_expr_ctx_t ctx
);

lou_sema_value_t *lou_sema_decl_ensure_initialized(lou_sema_t *sema, lou_sema_decl_t *decl);

void lou_sema_push_scope_frame(lou_sema_t *sema, lou_sema_scope_frame_t *frame);
lou_sema_scope_frame_t *lou_sema_pop_scope_frame(lou_sema_t *sema);
void lou_sema_push_scope(lou_sema_t *sema);
lou_sema_scope_t *lou_sema_pop_scope(lou_sema_t *sema);
lou_hir_local_t *lou_sema_add_final(lou_sema_t *sema, lou_sema_type_t *type);
lou_sema_type_t *lou_sema_func_returns(lou_sema_t *sema);

lou_sema_type_t *lou_sema_default_integer_type(lou_sema_t *sema);

void lou_sema_push_stmt(lou_sema_t *sema, lou_hir_stmt_t *stmt);

#define LOU_SEMA_EXPECT_NOT_NULL(SEMA, AT, VALUE, MESSAGE, ...) ({ \
  typeof(VALUE) __value = VALUE; \
  if (!__value) { \
    lou_sema_err(SEMA, AT, MESSAGE, ##__VA_ARGS__); \
    return NULL; \
  } \
  __value; \
})

