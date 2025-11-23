#pragma once

#include "core/slice.h"
#include <stdbool.h>

typedef struct ast_expr_t ast_expr_t;
typedef struct ast_type_t ast_type_t;

typedef enum {
  AST_DECL_META,
  AST_DECL_CONST,
  AST_DECL_FINAL,
  AST_DECL_VAR,
} ast_decl_kind_t;

typedef struct {
  bool is_public;
  ast_decl_kind_t kind;
  slice_t name;
  ast_type_t *type;
  ast_expr_t *initializer;
} ast_decl_t;

