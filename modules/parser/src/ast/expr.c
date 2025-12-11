#include "lou/parser/ast/expr.h"

lou_ast_struct_field_t lou_ast_struct_field_new(lou_slice_t name, lou_ast_expr_t *type) {
  return (lou_ast_struct_field_t) {
    .name = name,
    .type = type,
  };
}

lou_ast_expr_t *lou_ast_expr_new_ident(lou_mempool_t *mempool, lou_slice_t ident) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_IDENT;
  expr->slice = ident;
  expr->ident = ident;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_unary(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_unary_t kind, lou_ast_expr_t *inner) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_UNARY;
  expr->unary.inner = inner;
  expr->unary.kind = kind;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_get_ident(lou_mempool_t *mempool, lou_ast_expr_t *inner, lou_slice_t ident) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_GET_IDENT;
  expr->slice = ident;
  expr->get_ident.ident = ident;
  expr->get_ident.inner = inner;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_array(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_expr_t **values) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_ARRAY;
  expr->slice = slice;
  expr->array.values = values;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_call(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_expr_t *inner, lou_ast_expr_t **args) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_CALL;
  expr->slice = slice;
  expr->call.inner = inner;
  expr->call.args = args;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_integer(lou_mempool_t *mempool, lou_slice_t slice, uint64_t integer) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_INTEGER;
  expr->slice = slice;
  expr->integer = integer;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_string(lou_mempool_t *mempool, lou_slice_t slice, lou_slice_t string, lou_token_string_kind_t kind) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_STRING;
  expr->slice = slice;
  expr->string.content = string;
  expr->string.kind = kind;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_char(lou_mempool_t *mempool, lou_slice_t slice, char character) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_CHAR;
  expr->slice = slice;
  expr->character = character;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_assign(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_expr_t *to, lou_ast_expr_t *what) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_ASSIGN;
  expr->slice = slice;
  expr->assign.to = to;
  expr->assign.what = what;
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

lou_ast_expr_t *lou_ast_expr_new_struct_type(
  lou_mempool_t *mempool,
  lou_slice_t slice,
  lou_ast_struct_field_t *fields
) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_STRUCT_TYPE;
  expr->slice = slice;
  expr->struct_type.fields = fields;
  return expr;
}


lou_ast_expr_t *lou_ast_expr_new_binop(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_binop_t kind, lou_ast_expr_t *left, lou_ast_expr_t *right) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_BINOP;
  expr->slice = slice;
  expr->binop.kind = kind;
  expr->binop.left = left;
  expr->binop.right = right;
  return expr;
}

lou_ast_expr_t *lou_ast_expr_new_scope(lou_mempool_t *mempool, lou_slice_t slice, lou_ast_expr_t *inner) {
  lou_ast_expr_t *expr = LOU_MEMPOOL_ALLOC(mempool, lou_ast_expr_t);
  expr->kind = LOU_AST_EXPR_SCOPE;
  expr->slice = slice;
  expr->scope_inner = inner;
  return expr;
}
