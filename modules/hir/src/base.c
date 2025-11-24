#include "lou/core/vec.h"
#include "lou/hir/hir.h"
#include "lou/core/mempool.h"
#include "hir.h"

lou_hir_t *lou_hir_new(lou_mempool_t *mempool) {
  lou_hir_t *ir = LOU_MEMPOOL_ALLOC(mempool, lou_hir_t);
  ir->decls = LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_decl_t*);
  return ir;
}

void lou_hir_decl_add(lou_hir_t *ir, lou_hir_decl_t *decl) {
  *LOU_VEC_PUSH(&ir->decls) = decl;
}
