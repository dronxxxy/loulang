#include "lou/ir/func.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"

bool lou_ir_func_valid(lou_ir_func_t *func) {
  return func->code != NULL;
}

lou_ir_local_t *lou_ir_func_local_add(lou_mempool_t *mempool, lou_ir_func_t *func, lou_ir_local_t value) {
  lou_ir_local_t *local = LOU_MEMPOOL_ALLOC(mempool, lou_ir_local_t); 
  *local = value;
  *LOU_VEC_PUSH(&func->locals) = local;
  return local;
}

void lou_ir_func_init(lou_ir_func_t *func, lou_ir_code_t *code) {
  func->code = code;
}
