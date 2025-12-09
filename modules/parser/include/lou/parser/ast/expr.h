#pragma once

#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/lexer/token.h"
#include "lou/parser/ast/body.h"
#include <stdint.h>

typedef struct lou_ast_expr_t lou_ast_expr_t;

typedef struct {
  lou_slice_t name;
  lou_ast_expr_t *type;
} lou_ast_expr_func_arg_t;

typedef enum {
  LOU_AST_BINOP_ADD,
  LOU_AST_BINOP_SUBTRACT,
  LOU_AST_BINOP_MULTIPLY,
  LOU_AST_BINOP_DIVIDE,
  LOU_AST_BINOP_MOD,
} lou_ast_binop_t;

typedef enum {
  LOU_AST_EXPR_IDENT,
  LOU_AST_EXPR_GET_IDENT,
  LOU_AST_EXPR_FUNC,
  LOU_AST_EXPR_INTEGER,
  LOU_AST_EXPR_STRING,
  LOU_AST_EXPR_CALL,
  LOU_AST_EXPR_ARRAY,
  LOU_AST_EXPR_CHAR,
  LOU_AST_EXPR_ASSIGN,
  LOU_AST_EXPR_BINOP,
} lou_ast_expr_kind_t;

// this is bad, of course, but who cares?)
typedef struct lou_sema_value_t lou_sema_value_t;

typedef struct lou_ast_expr_t {
  lou_ast_expr_kind_t kind;
  lou_slice_t slice;

  union {
    lou_slice_t ident;
    char character;
    uint64_t integer;

    struct {
      lou_ast_expr_func_arg_t *args;
      lou_ast_expr_t *returns;
      lou_ast_body_t *body;
      lou_slice_t fun_slice;
    } func;

    struct {
      lou_ast_expr_t *inner; 
      lou_slice_t ident;
    } get_ident;

    struct {
      lou_ast_expr_t **values; 
    } array;

    struct {
      lou_ast_expr_t *inner;
      lou_ast_expr_t **args;
    } call;

    struct {
      lou_slice_t content;
      lou_token_string_kind_t kind;
    } string;

    struct {
      lou_ast_expr_t *to;
      lou_ast_expr_t *what;
    } assign;

    struct {
      lou_ast_binop_t kind;
      lou_ast_expr_t *left;
      lou_ast_expr_t *right;
    } binop;
  };

  lou_sema_value_t *sema_value;
} lou_ast_expr_t;

lou_ast_expr_t *lou_ast_expr_new_ident(lou_mempool_t *mempool, lou_slice_t ident);
lou_ast_expr_t *lou_ast_expr_new_get_ident(lou_mempool_t *mempool, lou_ast_expr_t *inner, lou_slice_t ident);
lou_ast_expr_t *lou_ast_expr_new_array(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_expr_t **values);
lou_ast_expr_t *lou_ast_expr_new_call(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_expr_t *inner, lou_ast_expr_t **args);
lou_ast_expr_t *lou_ast_expr_new_integer(lou_mempool_t *mempool, lou_slice_t slice, uint64_t integer);
lou_ast_expr_t *lou_ast_expr_new_char(lou_mempool_t *mempool, lou_slice_t slice, char character);
lou_ast_expr_t *lou_ast_expr_new_string(lou_mempool_t *mempool, lou_slice_t slice, lou_slice_t string, lou_token_string_kind_t kind);
lou_ast_expr_t *lou_ast_expr_new_assign(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_expr_t *to, lou_ast_expr_t *what);
lou_ast_expr_t *lou_ast_expr_new_binop(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_binop_t kind, lou_ast_expr_t *left, lou_ast_expr_t *right);
lou_ast_expr_t *lou_ast_expr_new_func(
  lou_mempool_t *mempool,
  lou_slice_t slice,
  lou_ast_expr_func_arg_t *args,
  lou_ast_expr_t *returns,
  lou_ast_body_t *body
);
