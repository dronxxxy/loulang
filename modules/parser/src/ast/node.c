#include "lou/parser/ast/node.h"

lou_ast_node_t *lou_ast_node_new_decl(lou_mempool_t *mempool, lou_ast_decl_t decl) {
  lou_ast_node_t *node = LOU_MEMPOOL_ALLOC(mempool, lou_ast_node_t);
  node->kind = LOU_AST_NODE_DECL;
  node->decl = decl;
  return node;
}

lou_ast_node_t *lou_ast_node_new_expr(lou_mempool_t *mempool, lou_ast_expr_t *expr) {
  lou_ast_node_t *node = LOU_MEMPOOL_ALLOC(mempool, lou_ast_node_t);
  node->kind = LOU_AST_NODE_EXPR;
  node->expr = expr;
  return node;
}


