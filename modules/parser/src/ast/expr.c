#include "lou/parser/ast/expr.h"

lou_ast_expr_t *lou_ast_expr_new_ident(lou_mempool_t *mempool, lou_slice_t ident) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_IDENT;
  expr->slice = ident;
  expr->ident = ident;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_get_ident(lou_mempool_t *mempool, lou_ast_expr_get_ident_t get_ident) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_GET_IDENT;
  expr->slice = get_ident.ident;
  expr->get_ident = get_ident;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_array(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_expr_array_t array) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_ARRAY;
  expr->slice = slice;
  expr->array = array;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_call(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_expr_call_t call) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_CALL;
  expr->slice = slice;
  expr->call = call;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_integer(lou_mempool_t *mempool, lou_slice_t slice, uint64_t integer) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_INTEGER;
  expr->slice = slice;
  expr->integer = integer;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_string(lou_mempool_t *mempool, lou_slice_t slice, lou_slice_t string) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_STRING;
  expr->slice = slice;
  expr->string = string;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_char(lou_mempool_t *mempool, lou_slice_t slice, char character) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_CHAR;
  expr->slice = slice;
  expr->character = character;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_func(
  lou_mempool_t *mempool,
  lou_slice_t slice,
  lou_ast_expr_func_arg_t *args,
  lou_ast_expr_t *returns,
  lou_ast_body_t *body
) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_FUNC;
  expr->slice = slice;
  expr->func.args = args;
  expr->func.returns = returns;
  expr->func.body = body;
  return expr;
}

