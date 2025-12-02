#include "const.h"

lou_sema_const_t *lou_sema_const_new_func(lou_mempool_t *mempool, lou_sema_type_t *type, lou_sema_const_func_t func) {
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_FUNCTION;
  constant->func = func;
  return constant;
}

lou_sema_const_t *lou_sema_const_new_integer(lou_mempool_t *mempool, lou_sema_type_t *type, uint64_t integer) {
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_INTEGER;
  constant->integer = integer;
  return constant;
}

lou_sema_const_t *lou_sema_const_new_string(lou_mempool_t *mempool, lou_sema_type_t *type, lou_slice_t string) {
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_STRING;
  constant->string = string;
  return constant;
}


