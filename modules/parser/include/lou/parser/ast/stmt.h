#pragma once

#include "lou/core/mempool.h"

typedef struct lou_ast_expr_t lou_ast_expr_t;

typedef struct {
  lou_ast_expr_t *value;
} lou_ast_stmt_ret_t;

typedef enum {
  LOU_AST_STMT_RETURN,
  LOU_AST_STMT_EXPR,
} lou_ast_stmt_kind_t;

typedef struct lou_ast_stmt_t {
  lou_ast_stmt_kind_t kind;

  union {
    lou_ast_stmt_ret_t ret;
    lou_ast_expr_t *expr;
  };
} lou_ast_stmt_t;

static inline lou_ast_stmt_t *lou_ast_stmt_new_expr(lou_mempool_t *mempool, lou_ast_expr_t *expr) {
  lou_ast_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_ast_stmt_t);
  stmt->kind = LOU_AST_STMT_EXPR;
  stmt->expr = expr;
  return stmt;
}

static inline lou_ast_stmt_t *lou_ast_stmt_new_ret(lou_mempool_t *mempool, lou_ast_stmt_ret_t ret) {
  lou_ast_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_ast_stmt_t);
  stmt->kind = LOU_AST_STMT_RETURN;
  stmt->ret = ret;
  return stmt;
}
