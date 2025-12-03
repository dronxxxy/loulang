#include "lou/parser/ast/body.h"

lou_ast_body_t *lou_ast_body_new(lou_mempool_t *mempool, lou_ast_stmt_t **stmts) {
  lou_ast_body_t *body = LOU_MEMPOOL_ALLOC(mempool, lou_ast_body_t);
  body->stmts = stmts;
  return body;
}

