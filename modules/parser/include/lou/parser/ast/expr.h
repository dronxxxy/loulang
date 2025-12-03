#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/parser/ast/body.h"
#include <stdint.h>

typedef struct lou_ast_expr_t lou_ast_expr_t;

typedef struct {
  lou_ast_expr_t *inner;
  lou_ast_expr_t **args;
} lou_ast_expr_call_t;

typedef struct {
  lou_slice_t name;
  lou_ast_expr_t *type;
} lou_ast_expr_func_arg_t;

typedef struct {
  lou_ast_expr_func_arg_t *args;
  lou_ast_expr_t *returns;
  lou_ast_body_t *body;
  lou_slice_t fun_slice;
} lou_ast_expr_func_t;

typedef struct {
  lou_ast_expr_t *inner; 
  lou_slice_t ident;
} lou_ast_expr_get_ident_t;

typedef struct {
  lou_ast_expr_t **values; 
} lou_ast_expr_array_t;

typedef enum {
  LOU_AST_EXPR_IDENT,
  LOU_AST_EXPR_GET_IDENT,
  LOU_AST_EXPR_FUNC,
  LOU_AST_EXPR_INTEGER,
  LOU_AST_EXPR_STRING,
  LOU_AST_EXPR_CALL,
  LOU_AST_EXPR_ARRAY,
} lou_ast_expr_kind_t;

typedef struct lou_ast_expr_t {
  lou_ast_expr_kind_t kind;
  lou_slice_t slice;

  union {
    lou_slice_t ident;
    lou_slice_t string;
    lou_ast_expr_func_t func;
    uint64_t integer;
    lou_ast_expr_call_t call;
    lou_ast_expr_t **elements; 
    lou_ast_expr_get_ident_t get_ident;
    lou_ast_expr_array_t array;
  };
} lou_ast_expr_t;

lou_ast_expr_t *lou_ast_expr_new_ident(lou_mempool_t *mempool, lou_slice_t ident);
lou_ast_expr_t *lou_ast_expr_new_get_ident(lou_mempool_t *mempool, lou_ast_expr_get_ident_t get_ident);
lou_ast_expr_t *lou_ast_expr_new_array(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_expr_array_t array);
lou_ast_expr_t *lou_ast_expr_new_call(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_expr_call_t call);
lou_ast_expr_t *lou_ast_expr_new_integer(lou_mempool_t *mempool, lou_slice_t slice, uint64_t integer);
lou_ast_expr_t *lou_ast_expr_new_string(lou_mempool_t *mempool, lou_slice_t slice, lou_slice_t string);
lou_ast_expr_t *lou_ast_expr_new_func(
  lou_mempool_t *mempool,
  lou_slice_t slice,
  lou_ast_expr_func_arg_t *args,
  lou_ast_expr_t *returns,
  lou_ast_body_t *body
);
