#pragma once

#include "lou/core/mempool.h"

typedef struct lou_ast_body_t {
} lou_ast_body_t;

static inline lou_ast_body_t *lou_ast_body_new(lou_mempool_t *mempool) {
  lou_ast_body_t *body = LOU_MEMPOOL_ALLOC(mempool, lou_ast_body_t);
  return body;
}
