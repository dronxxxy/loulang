#pragma once

#include "lou/core/mempool.h"
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

typedef struct lou_sema_t {
  lou_parser_t *parser;
  lou_mempool_t *mempool;
  lou_ast_node_t **nodes;
  bool failed;
  
  lou_ast_node_t **node_stack;
  lou_sema_decl_t **global_decls;
  lou_sema_scope_frame_t **scope_frames;
} lou_sema_t;

void lou_sema_init_decl(lou_sema_decl_t *decl, lou_sema_value_t *value);

lou_sema_decl_t *lou_sema_add_decl(
  lou_sema_t *sema,
  bool is_public,
  lou_slice_t name,
  lou_sema_decl_kind_t kind,
  lou_ast_node_t *prefetch_node
);

void lou_sema_err(lou_sema_t *sema, lou_slice_t slice, const char *fmt, ...);
lou_sema_value_t *lou_sema_resolve(lou_sema_t *sema, lou_slice_t name);
lou_sema_value_t *lou_sema_call_plugin(lou_sema_t *sema, lou_sema_plugin_t *plugin, lou_slice_t slice, lou_slice_t *arg_slices, lou_sema_value_t **args);
lou_sema_type_t *lou_sema_expect_type(lou_sema_t *sema, lou_slice_t at, lou_sema_value_t *value);

void lou_sema_push_scope_frame(lou_sema_t *sema);
void lou_sema_pop_scope_frame(lou_sema_t *sema);
void lou_sema_push_scope(lou_sema_t *sema);
void lou_sema_pop_scope(lou_sema_t *sema);

lou_sema_type_t *lou_sema_default_integer_type(lou_sema_t *sema);

