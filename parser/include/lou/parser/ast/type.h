#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"

typedef struct lou_ast_expr_t lou_ast_expr_t;
typedef struct lou_ast_type_t lou_ast_type_t;

typedef struct {
  lou_opt_slice_t name;
  lou_ast_type_t *type;
} lou_ast_type_func_arg_t;

typedef struct {
  lou_ast_type_func_arg_t *args;
  lou_ast_type_t *returns;
} lou_ast_type_func_t;

typedef enum {
  LOU_AST_TYPE_EXPR,
  LOU_AST_TYPE_FUNC,
} lou_ast_type_kind_t;

typedef struct lou_ast_type_t {
  lou_ast_type_kind_t kind;

  union {
    lou_ast_expr_t *expr;
    lou_ast_type_func_t func;
  };
} lou_ast_type_t;

static inline lou_ast_type_t *lou_ast_type_new_expr(lou_mempool_t *mempool, lou_ast_expr_t *expr) {
  lou_ast_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_ast_type_t);
  type->kind = LOU_AST_TYPE_EXPR;
  type->expr = expr;
  return type;
}

static inline lou_ast_type_t *lou_ast_type_new_func(lou_mempool_t *mempool, lou_ast_type_func_t func) {
  lou_ast_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_ast_type_t);
  type->kind = LOU_AST_TYPE_EXPR;
  type->func = func;
  return type;
}
