#include "lou/core/vec.h"
#include "lou/ir/ir.h"
#include "lou/core/mempool.h"
#include "ir.h"

lou_ir_t *lou_ir_new(lou_mempool_t *mempool) {
  lou_ir_t *ir = LOU_MEMPOOL_ALLOC(mempool, lou_ir_t);
  ir->decls = LOU_MEMPOOL_VEC_NEW(mempool, lou_ir_decl_t*);
  return ir;
}

void lou_ir_decl_add(lou_ir_t *ir, lou_ir_decl_t *decl) {
  *LOU_VEC_PUSH(&ir->decls) = decl;
}
