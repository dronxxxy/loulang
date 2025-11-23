#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/parser/ast/body.h"

typedef struct lou_ast_expr_t lou_ast_expr_t;
typedef struct lou_ast_type_t lou_ast_type_t;

typedef struct {
  lou_slice_t name;
  lou_ast_type_t *type;
} lou_ast_expr_func_arg_t;

typedef struct {
  lou_ast_expr_func_arg_t *args;
  lou_ast_type_t *returns;
  lou_ast_body_t *body;
} lou_ast_expr_func_t;

typedef enum {
  LOU_AST_EXPR_IDENT,
  LOU_AST_EXPR_FUNC,
} lou_ast_expr_kind_t;

typedef struct lou_ast_expr_t {
  lou_ast_expr_kind_t kind;

  union {
    lou_slice_t ident;
    lou_ast_expr_func_t func;
  };
} lou_ast_expr_t;

static inline lou_ast_expr_t *lou_ast_expr_new_ident(lou_mempool_t *mempool, lou_slice_t ident) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_IDENT;
  expr->ident = ident;
  return expr;
}

static inline lou_ast_expr_t *lou_ast_expr_new_func(lou_mempool_t *mempool, lou_ast_expr_func_t func) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_IDENT;
  expr->func = func;
  return expr;
}
