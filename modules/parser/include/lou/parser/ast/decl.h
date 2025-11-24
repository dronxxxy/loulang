#pragma once

#include "lou/core/slice.h"
#include <stdbool.h>

typedef struct lou_ast_expr_t lou_ast_expr_t;
typedef struct lou_ast_type_t lou_ast_type_t;

typedef enum {
  LOU_AST_DECL_META,
  LOU_AST_DECL_CONST,
  LOU_AST_DECL_FINAL,
  LOU_AST_DECL_VAR,
} lou_ast_decl_kind_t;

typedef enum {
  LOU_AST_DECL_LINK_NONE = 0,
  LOU_AST_DECL_LINK_GLOBAL,
  LOU_AST_DECL_LINK_EXTERN,
} lou_ast_decl_link_kind_t;

typedef struct {
  bool is_public;
  lou_ast_decl_link_kind_t link_kind;
  lou_ast_decl_kind_t kind;
  lou_slice_t name;
  lou_ast_type_t *type;
  lou_ast_expr_t *initializer;
} lou_ast_decl_t;

