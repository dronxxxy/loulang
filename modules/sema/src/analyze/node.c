#include "node.h"
#include "analyze/expr.h"
#include "lou/core/assertions.h"
#include "lou/core/vec.h"
#include "lou/parser/ast/decl.h"
#include "sema.h"

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

bool lou_sema_analyze_node(lou_sema_t *sema, lou_ast_node_t *node) {
  for (size_t i = 0; i < lou_vec_len(sema->node_stack); i++) {
    if (sema->node_stack[i] == node) {
      lou_sema_err(sema, lou_sema_node_slice(node), "circular reference found!");
      return false;
    }
  }
  *LOU_VEC_PUSH(&sema->node_stack) = node;
  switch (node->kind) {
    case LOU_AST_NODE_DECL:
      lou_sema_init_decl(node->decl.sema, lou_sema_analyze_expr(sema, node->decl.initializer));
      break;
  }
  return true;
}

lou_slice_t lou_sema_node_slice(lou_ast_node_t *node) {
  switch (node->kind) {
    case LOU_AST_NODE_DECL: return node->decl.name;
  }
  UNREACHABLE();
}
