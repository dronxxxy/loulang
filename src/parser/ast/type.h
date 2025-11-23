#pragma once

#include "core/mempool.h"
#include "core/slice.h"

typedef struct ast_expr_t ast_expr_t;
typedef struct ast_type_t ast_type_t;

typedef struct {
  opt_slice_t name;
  ast_type_t *type;
} ast_type_arg_t;

typedef struct {
  ast_type_arg_t *args;
  ast_type_t *returns;
} ast_type_func_t;

typedef enum {
  AST_TYPE_EXPR,
  AST_TYPE_FUNC,
} ast_type_kind_t;

typedef struct ast_type_t {
  ast_type_kind_t kind;

  union {
    ast_expr_t *expr;
    ast_type_func_t func;
  };
} ast_type_t;

static inline ast_type_t *ast_type_new_expr(mempool_t *mempool, ast_expr_t *expr) {
  ast_type_t *type = MEMPOOL_ALLOC(mempool, ast_type_t);
  type->kind = AST_TYPE_EXPR;
  type->expr = expr;
  return type;
}

static inline ast_type_t *ast_type_new_func(mempool_t *mempool, ast_type_func_t func) {
  ast_type_t *type = MEMPOOL_ALLOC(mempool, ast_type_t);
  type->kind = AST_TYPE_EXPR;
  type->func = func;
  return type;
}
