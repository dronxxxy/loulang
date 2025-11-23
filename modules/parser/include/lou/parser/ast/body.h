#pragma once

#include "lou/core/mempool.h"

typedef struct lou_ast_stmt_t lou_ast_stmt_t;

typedef struct lou_ast_body_t {
  lou_ast_stmt_t **stmts;
} lou_ast_body_t;

static inline lou_ast_body_t *lou_ast_body_new(lou_mempool_t *mempool, lou_ast_stmt_t **stmts) {
  lou_ast_body_t *body = LOU_MEMPOOL_ALLOC(mempool, lou_ast_body_t);
  body->stmts = stmts;
  return body;
}
