#pragma once

#include "core/mempool.h"
#include "parser/ast/decl.h"
#include <stdbool.h>

typedef struct ast_expr_t ast_expr_t;

typedef enum {
  AST_NODE_DECL,
} ast_node_kind_t;

typedef struct {
  ast_node_kind_t kind;

  union {
    ast_decl_t decl;
  };
} ast_node_t;

static inline ast_node_t *ast_node_new_decl(mempool_t *mempool, ast_decl_t decl) {
  ast_node_t *node = MEMPOOL_ALLOC(mempool, ast_node_t);
  node->kind = AST_NODE_DECL;
  node->decl = decl;
  return node;
}

