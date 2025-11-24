#include "lou/hir/func.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"

bool lou_hir_func_valid(lou_hir_func_t *func) {
  return func->code != NULL;
}

lou_hir_local_t *lou_hir_func_local_add(lou_mempool_t *mempool, lou_hir_func_t *func, lou_hir_local_t value) {
  lou_hir_local_t *local = LOU_MEMPOOL_ALLOC(mempool, lou_hir_local_t); 
  *local = value;
  *LOU_VEC_PUSH(&func->locals) = local;
  return local;
}

void lou_hir_func_init(lou_hir_func_t *func, lou_hir_code_t *code) {
  func->code = code;
}
