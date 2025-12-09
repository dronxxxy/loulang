#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"

typedef struct lou_ast_expr_t lou_ast_expr_t;
typedef struct lou_ast_node_t lou_ast_node_t;
typedef struct lou_ast_body_t lou_ast_body_t;

typedef enum {
  LOU_AST_STMT_RETURN,
  LOU_AST_STMT_NODE,
  LOU_AST_STMT_IF,
} lou_ast_stmt_kind_t;

typedef struct lou_ast_stmt_t {
  lou_ast_stmt_kind_t kind;
  lou_slice_t slice;

  union {
    lou_ast_node_t *node;

    struct {
      lou_slice_t slice;
      lou_ast_expr_t *value;
    } ret;

    struct {
      lou_ast_expr_t *condition;
      lou_ast_body_t *body;
      lou_ast_body_t *else_body;
    } if_else;
  };
} lou_ast_stmt_t;

lou_ast_stmt_t *lou_ast_stmt_new_if(
  lou_mempool_t *mempool,
  lou_slice_t slice,
  lou_ast_expr_t *condition,
  lou_ast_body_t *body,
  lou_ast_body_t *else_body
);
lou_ast_stmt_t *lou_ast_stmt_new_node(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_node_t *node);
lou_ast_stmt_t *lou_ast_stmt_new_ret(lou_mempool_t *mempool, lou_slice_t slice, lou_slice_t ret_slice, lou_ast_expr_t *value);
