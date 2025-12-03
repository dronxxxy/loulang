#include "lou/hir/func.h"
#include "lou/core/mempool.h"
#include "lou/hir/code.h"

bool lou_hir_func_valid(lou_hir_func_t *func) {
  return func->code != NULL;
}

void lou_hir_func_set_global(lou_hir_func_t *func, lou_slice_t name) {
  func->name = (lou_opt_slice_t) { true, name };
}

lou_hir_func_t *lou_hir_func_new(lou_mempool_t *mempool) {
  lou_hir_func_t *func = LOU_MEMPOOL_ALLOC(mempool, lou_hir_func_t);
  func->name = (lou_opt_slice_t) { false };
  func->code = lou_hir_code_new(mempool);
  return func;
}
