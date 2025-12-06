#include "node.h"
#include "ast/expr.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "type.h"
#include "value.h"

lou_sema_decl_t *lou_sema_declare_node(lou_sema_t *sema, lou_ast_node_t *node) {
  switch (node->kind) {
    case LOU_AST_NODE_DECL: return lou_sema_add_decl(sema, node->decl.is_public ? LOU_SEMA_PUBLIC : LOU_SEMA_PRIVATE, node->decl.name, node);
  }
  UNREACHABLE();
}

static inline void lou_sema_outline_meta_decl(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  if (node->decl.type) {
    lou_sema_err(sema, node->decl.type->slice, "a type must not be specified for the meta declaration");
  }
  if (!node->decl.initializer) {
    lou_sema_err(sema, node->decl.name, "meta declaration must be initialized");
    lou_sema_kill_decl(decl);
    return;
  }
  lou_sema_value_t *value = lou_sema_expr_outline(sema, node->decl.initializer, lou_sema_expr_ctx_new_comptime());
  if (value) {
    lou_sema_outline_decl(decl, value);
  } else {
    lou_sema_kill_decl(decl);
  }
}

static inline void lou_sema_outline_const_decl(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  lou_sema_type_t *type = node->decl.type ? lou_sema_expr_analyze_type(sema, node->decl.type, lou_sema_expr_ctx_new_comptime()) : NULL;
  if (!node->decl.initializer) {
    lou_sema_err(sema, node->decl.name, "constant declaration must be initialized");
    lou_sema_kill_decl(decl);
    return;
  }
  lou_sema_value_t *value = lou_sema_expr_outline_runtime(sema, node->decl.initializer, lou_sema_expr_ctx_new_runtime(type));
  if (!value) {
    lou_sema_kill_decl(decl);
    return;
  }

  lou_sema_type_t *value_type = lou_sema_value_is_runtime(value);
  if (type && !lou_sema_type_eq(type, value_type)) {
    lou_sema_err(sema, node->decl.initializer->slice, "expression of type #T was expected but it has type #T", type, value_type);
    lou_sema_kill_decl(decl);
    return;
  }

  lou_sema_outline_decl(decl, value);
}

static inline void lou_sema_outline_emittable_decl(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  lou_sema_type_t *type = NULL;

  if (node->decl.type) {
    type = lou_sema_expr_analyze_type(sema, node->decl.type, lou_sema_expr_ctx_new_comptime());
    if (!type) {
      lou_sema_kill_decl(decl);
      return;
    }
    // TODO: link to emittable declaration
    NOT_IMPLEMENTED;
  } else {
    lou_sema_value_t *value = lou_sema_expr_outline_runtime(sema, node->decl.initializer, lou_sema_expr_ctx_new_runtime(type));
    if (!value) {
      lou_sema_kill_decl(decl);
      return;
    }
    (void)value;
    // TODO: link to emittable declaration
    NOT_IMPLEMENTED;
  }
}

void lou_sema_outline_node(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  switch (node->kind) {
    case LOU_AST_NODE_DECL: {
      switch (node->decl.kind) {
        case LOU_AST_DECL_META: return lou_sema_outline_meta_decl(sema, node, decl);
        case LOU_AST_DECL_CONST: return lou_sema_outline_const_decl(sema, node, decl);
        case LOU_AST_DECL_FINAL: case LOU_AST_DECL_VAR: return lou_sema_outline_emittable_decl(sema, node, decl);
      }
      return;
    }
  }
  UNREACHABLE();
}

static inline void lou_sema_finalize_immutable_decl(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  if (lou_sema_expr_finalize(sema, node->decl.initializer, decl->value)) {
    lou_sema_finalize_decl(decl);
  } else {
    lou_sema_kill_decl(decl);
  }
}

static inline void lou_sema_finalize_emittable_decl(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  NOT_IMPLEMENTED;
}

void lou_sema_finalize_node(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  switch (node->kind) {
    case LOU_AST_NODE_DECL: {
      switch (node->decl.kind) {
        case LOU_AST_DECL_META: case LOU_AST_DECL_CONST: return lou_sema_finalize_immutable_decl(sema, node, decl);
        case LOU_AST_DECL_FINAL: case LOU_AST_DECL_VAR: return lou_sema_finalize_emittable_decl(sema, node, decl);
      }
      return;
    }
  }
  UNREACHABLE();
}
