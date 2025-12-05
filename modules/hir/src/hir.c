#include "lou/hir/hir.h"
#include "extern.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/hir/func.h"
#include "func.h"

lou_hir_t *lou_hir_new(lou_mempool_t *mempool) {
  lou_hir_t *ir = LOU_MEMPOOL_ALLOC(mempool, lou_hir_t);
  ir->mempool = mempool;
  ir->functions = LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_func_t*);
  ir->externs = LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_extern_t*);
  return ir;
}

lou_hir_func_t *lou_hir_func_add(lou_hir_t *ir, lou_hir_type_t *type) {
  lou_hir_func_t *func = lou_hir_func_new(ir->mempool);
  *LOU_VEC_PUSH(&ir->functions) = func;
  return  func;
}

lou_hir_extern_t *lou_hir_extern_add(lou_hir_t *ir, lou_hir_type_t *base_type, lou_slice_t name) {
  lou_hir_extern_t *external = lou_hir_extern_new(ir->mempool, base_type, name);
  *LOU_VEC_PUSH(&ir->externs) = external;
  return external;
}
