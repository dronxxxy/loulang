#include "lou/sema/sema.h"
#include "ast/node.h"
#include "impl.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/hir/hir.h"
#include "lou/parser/parser.h"
#include "lou/core/assertions.h"

lou_sema_t *lou_sema_new(lou_slice_t path) {
  lou_parser_t *parser = NOT_NULL(lou_parser_new(path));
  lou_mempool_t *mempool = lou_mempool_new();
  lou_sema_t *sema = LOU_MEMPOOL_ALLOC(mempool, lou_sema_t);
  sema->mempool = mempool;
  sema->global_decls = LOU_MEMPOOL_VEC_NEW(mempool, lou_sema_global_decl_t);
  sema->scope_stacks = LOU_MEMPOOL_VEC_NEW(mempool, lou_sema_scope_stack_t);
  sema->nodes = NULL;
  sema->hir = lou_hir_new(mempool);
  sema->parser = parser;
  sema->failed = false;
  return sema;
}

void lou_sema_free(lou_sema_t *sema) {
  lou_parser_free(sema->parser);
  lou_mempool_free(sema->mempool);
}

bool lou_sema_failed(const lou_sema_t *sema) {
  return sema->failed;
}

lou_hir_t *lou_sema_hir(const lou_sema_t *sema) {
  return sema->hir;
}

void lou_sema_read(lou_sema_t *sema) {
  assert(!sema->nodes);
  sema->nodes = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_node_record_t);

  lou_ast_node_t *node;
  while ((node = lou_parser_next(sema->parser))) {
    *LOU_VEC_PUSH(&sema->nodes) = (lou_sema_node_record_t) {
      .node = node,
      .decl = lou_sema_declare_node(sema, node),
    };
  }
}

void lou_sema_analyze(lou_sema_t *sema) {
  assert(sema->nodes);
  for (size_t i = 0; i < lou_vec_len(sema->nodes); i++) {
    // TODO: add currentNodeId for global search
    //       - declaration search begins at [currentNodeId, 0]
    //       - if not found, search in [nodes.length, currentNodeId + 1]
    lou_sema_node_record_t *record = &sema->nodes[i];
    if (record->decl->stage == LOU_SEMA_DECL_STAGE_KILLED) continue;
    lou_sema_outline_node(sema, record->node, record->decl);
    if (record->decl->stage == LOU_SEMA_DECL_STAGE_KILLED) continue;
    lou_sema_finalize_node(sema, record->node, record->decl);
  }
}

