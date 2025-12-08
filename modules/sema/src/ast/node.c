#include "node.h"
#include "ast/expr.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "lou/hir/code.h"
#include "type.h"
#include "value.h"
#include <assert.h>

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

static inline lou_sema_value_t *lou_sema_outline_immutable_decl(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  lou_sema_type_t *type = node->decl.type ? lou_sema_expr_analyze_type(sema, node->decl.type, lou_sema_expr_ctx_new_comptime(), false) : NULL;
  if (!node->decl.initializer) {
    lou_sema_err(sema, node->decl.name, "constant and final declarations must be initialized");
    lou_sema_kill_decl(decl);
    return NULL;
  }
  lou_sema_value_t *value = lou_sema_expr_outline_runtime(sema, node->decl.initializer, lou_sema_expr_ctx_new_runtime(type));
  if (!value) {
    lou_sema_kill_decl(decl);
    return NULL;
  }

  lou_sema_type_t *value_type = lou_sema_value_is_runtime(value);
  if (type && !lou_sema_type_eq(type, value_type)) {
    lou_sema_err(sema, node->decl.initializer->slice, "expression of type #T was expected but it has type #T", type, value_type);
    lou_sema_kill_decl(decl);
    return NULL;
  }
  return value;
}

static inline void lou_sema_outline_const_decl(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  lou_sema_value_t *value = RET_ON_NULL(lou_sema_outline_immutable_decl(sema, node, decl));
  if (!lou_sema_value_is_const(value)) {
    lou_sema_err(sema, node->decl.initializer->slice, "constant expression initializer must be constant but it is #V", value);
    lou_sema_kill_decl(decl);
    return;
  }
  lou_sema_outline_decl(decl, value);
}

static inline void lou_sema_outline_final_decl(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  lou_sema_value_t *value = RET_ON_NULL(lou_sema_outline_immutable_decl(sema, node, decl));
  if (lou_sema_is_global_scope(sema) && !lou_sema_value_is_const(value)) {
    lou_sema_err(sema, node->decl.initializer->slice, "expressions in global scope must be constant but it is #V", value);
    lou_sema_kill_decl(decl);
    return;
  }
  lou_sema_outline_decl(decl, value);
}

static inline lou_sema_value_t *lou_sema_push_var_decl(lou_sema_t *sema, lou_sema_type_t *type) {
  if (lou_sema_is_global_scope(sema)) {
    // TODO: global variables
    NOT_IMPLEMENTED;
  } else {
    return lou_sema_value_new_local(sema->mempool, LOU_SEMA_MUTABLE, type, lou_sema_add_local_var(sema, type), lou_sema_current_scope_stack(sema));
  }
}

static inline void lou_sema_outline_var_decl(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  lou_sema_type_t *type = NULL;

  if (node->decl.type) {
    type = lou_sema_expr_analyze_type(sema, node->decl.type, lou_sema_expr_ctx_new_comptime(), false);
    if (!type) {
      lou_sema_kill_decl(decl);
      return;
    }
  }
  lou_sema_value_t *value = NULL;
  if (node->decl.initializer) {
    value = lou_sema_expr_outline_runtime(sema, node->decl.initializer, lou_sema_expr_ctx_new_runtime(type));
    if (!value) {
      lou_sema_kill_decl(decl);
      return;
    }
    lou_sema_type_t *value_type = lou_sema_value_is_runtime(value);
    if (type && !lou_sema_type_eq(type, value_type)) {
      lou_sema_err(sema, node->decl.initializer->slice, "initializer of type #T was expected but got an expression of type #T", type, value_type);
      lou_sema_kill_decl(decl);
      return;
    }
    if (!type) type = value_type;
  }
  if (!type) {
    lou_sema_err(sema, node->decl.name, "the type or initializer should be specified");
    lou_sema_kill_decl(decl);
    return;
  }
  lou_sema_outline_decl(decl, lou_sema_push_var_decl(sema, type));
}

static inline void lou_sema_outline_node_internal(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  switch (node->kind) {
    case LOU_AST_NODE_DECL: {
      switch (node->decl.kind) {
        case LOU_AST_DECL_META: return lou_sema_outline_meta_decl(sema, node, decl);
        case LOU_AST_DECL_CONST: return lou_sema_outline_const_decl(sema, node, decl);
        case LOU_AST_DECL_FINAL: return lou_sema_outline_final_decl(sema, node, decl);
        case LOU_AST_DECL_VAR: return lou_sema_outline_var_decl(sema, node, decl);
      }
      return;
    }
  }
  UNREACHABLE();
}

void lou_sema_outline_node(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  lou_sema_push_analysing_node(sema, node);
  lou_sema_outline_node_internal(sema, node, decl);
  lou_sema_pop_analysing_node(sema);
}

static inline void lou_sema_finalize_var_decl(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  if (!node->decl.initializer) {
    lou_sema_finalize_decl(decl);
    return;
  }
  if (lou_sema_is_global_scope(sema)) {
    // TODO: global variables
    NOT_IMPLEMENTED;
  } else {
    lou_sema_value_t *value = lou_sema_expr_finalize(sema, node->decl.initializer, false);
    if (!value) {
      lou_sema_kill_decl(decl);
      return;
    }
    lou_sema_type_t *type = lou_sema_value_is_runtime(value);
    lou_hir_local_t *local = lou_sema_value_is_local(decl->value)->hir;
    if (!type) {
      lou_sema_err(sema, node->decl.name, "the type or initializer should be specified");
      lou_sema_kill_decl(decl);
      return;
    }
    lou_sema_push_stmt(sema, lou_hir_stmt_new_store_var(sema->mempool, local, lou_sema_value_as_hir(sema->mempool, value)));
    lou_sema_finalize_decl(decl);
  }
}

static inline void lou_sema_finalize_node_internal(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  switch (node->kind) {
    case LOU_AST_NODE_DECL: {
      switch (node->decl.kind) {
        case LOU_AST_DECL_VAR: lou_sema_finalize_var_decl(sema, node, decl); return;
        case LOU_AST_DECL_META:
        case LOU_AST_DECL_CONST:
        case LOU_AST_DECL_FINAL:
          if (node->decl.initializer) {
            if (lou_sema_expr_finalize(sema, node->decl.initializer, false)) {
              lou_sema_finalize_decl(decl);
            } else {
              lou_sema_kill_decl(decl);
            }
          }
          return;
      }
      UNREACHABLE();
    }
  }
  UNREACHABLE();
}

void lou_sema_finalize_node(lou_sema_t *sema, lou_ast_node_t *node, lou_sema_decl_t *decl) {
  lou_sema_push_analysing_node(sema, node);
  lou_sema_finalize_node_internal(sema, node, decl);
  lou_sema_pop_analysing_node(sema);
}
