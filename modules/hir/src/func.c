#include "lou/hir/func.h"
#include "lou/core/mempool.h"
#include "lou/hir/code.h"

void lou_hir_func_set_global(lou_hir_func_t *func, lou_slice_t name) {
  func->name = (lou_opt_slice_t) { true, name };
}

lou_hir_func_t *lou_hir_func_new(lou_mempool_t *mempool, lou_hir_type_t *type) {
  lou_hir_func_t *func = LOU_MEMPOOL_ALLOC(mempool, lou_hir_func_t);
  func->name = (lou_opt_slice_t) { false };
  func->code = NULL;
  func->type = type;
  return func;
}

void lou_hir_func_init(lou_hir_func_t *func, lou_hir_code_t *code) {
  func->code = code;
}
