#include "const.h"
#include "type.h"

lou_sema_const_t *lou_sema_const_new_int(lou_mempool_t *mempool, lou_sema_type_t *type, uint64_t value) {
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_INTEGER;
  constant->type = type;
  constant->integer = value;
  return constant;
}

lou_sema_const_t *lou_sema_const_new_func(lou_mempool_t *mempool, lou_sema_type_t *type, lou_hir_func_t *func) {
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_FUNC;
  constant->type = type;
  constant->func = func;
  return constant;
}

lou_sema_const_t *lou_sema_const_new_string(lou_mempool_t *mempool, lou_slice_t content) {
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_STRING;
  constant->type = lou_sema_type_new_string(mempool);
  constant->string = content;
  return constant;
}
