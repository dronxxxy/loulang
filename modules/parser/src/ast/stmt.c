#include "lou/parser/ast/stmt.h"

lou_ast_stmt_t *lou_ast_stmt_new_if(
  lou_mempool_t *mempool,
  lou_slice_t slice,
  lou_ast_expr_t *condition,
  lou_ast_body_t *body,
  lou_ast_body_t *else_body
) {
  lou_ast_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_ast_stmt_t);
  stmt->kind = LOU_AST_STMT_IF;
  stmt->slice = slice;
  stmt->if_else.body = body;
  stmt->if_else.condition = condition;
  stmt->if_else.else_body = else_body;
  return stmt;
}

lou_ast_stmt_t *lou_ast_stmt_new_node(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_node_t *node) {
  lou_ast_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_ast_stmt_t);
  stmt->kind = LOU_AST_STMT_NODE;
  stmt->slice = slice;
  stmt->node = node;
  return stmt;
}

lou_ast_stmt_t *lou_ast_stmt_new_ret(lou_mempool_t *mempool, lou_slice_t slice, lou_slice_t ret_slice, lou_ast_expr_t *value) {
  lou_ast_stmt_t *stmt = LOU_MEMPOOL_ALLOC(mempool, lou_ast_stmt_t);
  stmt->kind = LOU_AST_STMT_RETURN;
  stmt->slice = slice;
  stmt->ret.slice = ret_slice;
  stmt->ret.value = value;
  return stmt;
}
