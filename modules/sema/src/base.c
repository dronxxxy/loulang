#include "lou/core/vec.h"
#include "lou/sema/sema.h"
#include "lou/core/mempool.h"
#include "lou/parser/parser.h"
#include "sema.h"

lou_sema_t *lou_sema_new(lou_slice_t path) {
  lou_parser_t *parser = lou_parser_new(path);
  if (!parser) {
    return NULL;
  }
  lou_mempool_t *mempool = lou_mempool_new();
  lou_sema_t *sema = LOU_MEMPOOL_ALLOC(mempool, lou_sema_t);
  sema->parser = parser;
  sema->mempool = mempool;
  sema->nodes = LOU_MEMPOOL_VEC_NEW(mempool, lou_ast_node_t*);
  sema->failed = false;
  return sema;
}

void lou_sema_free(lou_sema_t *sema) {
  lou_parser_free(sema->parser);
  lou_mempool_free(sema->mempool);
}

bool lou_sema_failed(const lou_sema_t *sema) {
  return lou_parser_failed(sema->parser) || sema->failed;
}

void lou_sema_read(lou_sema_t *sema) {
  lou_ast_node_t *node;
  while ((node = lou_parser_next(sema->parser))) {
    *LOU_VEC_PUSH(&sema->nodes) = node;
  }
}

void lou_sema_analyze(lou_sema_t *sema) {
  // TODO: analyzing
}
