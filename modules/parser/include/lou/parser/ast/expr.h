#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/parser/ast/body.h"
#include <stdint.h>

typedef struct lou_ast_expr_t lou_ast_expr_t;
typedef struct lou_ast_type_t lou_ast_type_t;

typedef struct {
  lou_ast_expr_t *inner;
  lou_ast_expr_t **args;
} lou_ast_expr_call_t;

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
  LOU_AST_EXPR_INTEGER,
  LOU_AST_EXPR_CALL,
} lou_ast_expr_kind_t;

typedef struct lou_ast_expr_t {
  lou_ast_expr_kind_t kind;

  union {
    lou_slice_t ident;
    lou_ast_expr_func_t func;
    uint64_t integer;
    lou_ast_expr_call_t call;
  };
} lou_ast_expr_t;

static inline lou_ast_expr_t *lou_ast_expr_new_ident(lou_mempool_t *mempool, lou_slice_t ident) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_IDENT;
  expr->ident = ident;
  return expr;
}

static inline lou_ast_expr_t *lou_ast_expr_new_call(lou_mempool_t *mempool, lou_ast_expr_call_t call) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_IDENT;
  expr->call = call;
  return expr;
}

static inline lou_ast_expr_t *lou_ast_expr_new_integer(lou_mempool_t *mempool, uint64_t integer) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_IDENT;
  expr->integer = integer;
  return expr;
}


static inline lou_ast_expr_t *lou_ast_expr_new_func(lou_mempool_t *mempool, lou_ast_expr_func_t func) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_IDENT;
  expr->func = func;
  return expr;
}
