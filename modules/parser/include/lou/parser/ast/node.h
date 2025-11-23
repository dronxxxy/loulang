#pragma once

#include "lou/core/mempool.h"
#include "lou/parser/ast/decl.h"
#include <stdbool.h>

typedef struct lou_ast_expr_t lou_ast_expr_t;

typedef enum {
  LOU_AST_NODE_DECL,
} lou_ast_node_kind_t;

typedef struct lou_ast_node_t {
  lou_ast_node_kind_t kind;

  union {
    lou_ast_decl_t decl;
  };
} lou_ast_node_t;

static inline lou_ast_node_t *lou_ast_node_new_decl(lou_mempool_t *mempool, lou_ast_decl_t decl) {
  lou_ast_node_t *node = LOU_MEMPOOL_ALLOC(mempool, lou_ast_node_t);
  node->kind = LOU_AST_NODE_DECL;
  node->decl = decl;
  return node;
}

