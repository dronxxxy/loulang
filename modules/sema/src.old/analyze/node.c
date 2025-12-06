#include "node.h"
#include "analyze/expr.h"
#include "lou/core/assertions.h"
#include "lou/core/vec.h"
#include "lou/parser/ast/decl.h"
#include "impl.h"

static inline lou_sema_decl_kind_t lou_ast_decl_kind_to_sema(lou_ast_decl_kind_t kind) {
  switch (kind) {
    case LOU_AST_DECL_META: return LOU_SEMA_DECL_META;
    case LOU_AST_DECL_CONST: return LOU_SEMA_DECL_CONSTANT;
    case LOU_AST_DECL_FINAL: return LOU_SEMA_DECL_FINAL;
    case LOU_AST_DECL_VAR: return LOU_SEMA_DECL_VARIABLE;
  }
  UNREACHABLE();
}

void lou_sema_prefetch_node(lou_sema_t *sema, lou_ast_node_t *node) {
  switch (node->kind) {
    case LOU_AST_NODE_DECL:
      node->decl.sema = lou_sema_add_decl(sema, node->decl.is_public, node->decl.name, lou_ast_decl_kind_to_sema(node->decl.kind), node);
      return;
  }
  UNREACHABLE();
}

static inline bool lou_sema_just_analyze_node(lou_sema_t *sema, lou_ast_node_t *node) {
  switch (node->kind) {
    case LOU_AST_NODE_DECL: {
      lou_sema_type_t *type = node->decl.type ? lou_sema_analyze_type(sema, node->decl.type, false) : NULL;
      if (node->decl.kind == LOU_AST_DECL_META && type) {
        lou_sema_err(sema, node->decl.type->slice, "meta declaration should not have a type");
        type = NULL;
      }
      lou_sema_value_t *value = lou_sema_analyze_expr(sema, node->decl.initializer, lou_sema_expr_ctx_new(sema->mempool, type, true, false));
      lou_sema_init_decl(sema, node->decl.initializer->slice, node->decl.sema, value);
      if (!value) {
        return NULL;
      }
      lou_sema_type_t *value_type = lou_sema_value_is_runtime(value);
      if (type) {
        if (!value_type) {
          lou_sema_err(sema, node->decl.type->slice, "trying to assign comptime value #v to runtime declaration", value);
          return false;
        }
        if (!lou_sema_types_eq(type, value_type)) {
          lou_sema_err(sema, node->decl.type->slice, "types missmatch - expected #T got #T", type, value_type);
        }
      }
      break;
    }
  }
  return true;
}

static inline lou_ast_expr_t *lou_sema_node_initializer(lou_ast_node_t *node) {
  switch (node->kind) {
    case LOU_AST_NODE_DECL: return node->decl.initializer;
  }
  UNREACHABLE();
}

bool lou_sema_analyze_node(lou_sema_t *sema, lou_ast_node_t *node) {
  if (lou_sema_is_analysing_node(sema, node)) {
    lou_sema_err(sema, lou_sema_node_slice(node), "circular reference found!");
    return false;
  }
  lou_sema_push_analysing_node(sema, node, lou_sema_node_initializer(node));
  bool result = lou_sema_just_analyze_node(sema, node);
  lou_sema_pop_analysing_node(sema);
  return result;
}

lou_slice_t lou_sema_node_slice(lou_ast_node_t *node) {
  switch (node->kind) {
    case LOU_AST_NODE_DECL: return node->decl.name;
  }
  UNREACHABLE();
}
