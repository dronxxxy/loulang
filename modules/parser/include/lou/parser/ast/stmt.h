#pragma once

#include "lou/core/mempool.h"

typedef struct lou_ast_expr_t lou_ast_expr_t;
typedef struct lou_ast_node_t lou_ast_node_t;
typedef struct lou_ast_body_t lou_ast_body_t;

typedef struct {
  lou_ast_expr_t *condition;
  lou_ast_body_t *body;
} lou_ast_stmt_if_t;

typedef struct {
  lou_ast_expr_t *value;
} lou_ast_stmt_ret_t;

typedef enum {
  LOU_AST_STMT_RETURN,
  LOU_AST_STMT_EXPR,
  LOU_AST_STMT_NODE,
  LOU_AST_STMT_IF,
} lou_ast_stmt_kind_t;

typedef struct lou_ast_stmt_t {
  lou_ast_stmt_kind_t kind;

  union {
    lou_ast_stmt_ret_t ret;
    lou_ast_expr_t *expr;
    lou_ast_node_t *node;
    lou_ast_stmt_if_t if_else;
  };
} lou_ast_stmt_t;

static inline lou_ast_stmt_t *lou_ast_stmt_new_expr(lou_mempool_t *mempool, lou_ast_expr_t *expr) {
  lou_ast_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_ast_stmt_t);
  stmt->kind = LOU_AST_STMT_EXPR;
  stmt->expr = expr;
  return stmt;
}

static inline lou_ast_stmt_t *lou_ast_stmt_new_if(lou_mempool_t *mempool, lou_ast_stmt_if_t if_else) {
  lou_ast_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_ast_stmt_t);
  stmt->kind = LOU_AST_STMT_IF;
  stmt->if_else = if_else;
  return stmt;
}

static inline lou_ast_stmt_t *lou_ast_stmt_new_node(lou_mempool_t *mempool, lou_ast_node_t *node) {
  lou_ast_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_ast_stmt_t);
  stmt->kind = LOU_AST_STMT_NODE;
  stmt->node = node;
  return stmt;
}

static inline lou_ast_stmt_t *lou_ast_stmt_new_ret(lou_mempool_t *mempool, lou_ast_stmt_ret_t ret) {
  lou_ast_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_ast_stmt_t);
  stmt->kind = LOU_AST_STMT_RETURN;
  stmt->ret = ret;
  return stmt;
}
