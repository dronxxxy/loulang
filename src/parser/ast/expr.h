#pragma once

#include "core/mempool.h"
#include "core/slice.h"

typedef struct ast_expr_t ast_expr_t;
typedef struct ast_type_t ast_type_t;

typedef struct {
  slice_t name;
  ast_type_t *type;
} ast_expr_func_arg_t;

typedef struct {
  ast_expr_func_arg_t *args;
  ast_type_t *returns;
} ast_expr_func_t;

typedef enum {
  AST_EXPR_IDENT,
  AST_EXPR_FUNC,
} ast_expr_kind_t;

typedef struct ast_expr_t {
  ast_expr_kind_t kind;

  union {
    slice_t ident;
    ast_expr_func_t func;
  };
} ast_expr_t;

static inline ast_expr_t *ast_expr_new_ident(mempool_t *mempool, slice_t ident) {
  ast_expr_t *expr = MEMPOOL_ALLOC(mempool, ast_expr_t);
  expr->kind = AST_EXPR_IDENT;
  expr->ident = ident;
  return expr;
}

static inline ast_expr_t *ast_expr_new_func(mempool_t *mempool, ast_expr_func_t func) {
  ast_expr_t *expr = MEMPOOL_ALLOC(mempool, ast_expr_t);
  expr->kind = AST_EXPR_IDENT;
  expr->func = func;
  return expr;
}
