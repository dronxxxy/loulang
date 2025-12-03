#include "const.h"
#include "type.h"
#include <assert.h>

lou_sema_const_t *lou_sema_const_new_func(lou_mempool_t *mempool, lou_sema_type_t *type, lou_hir_func_t *func) {
  assert(type->kind == LOU_SEMA_TYPE_FUNCTION);
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_FUNCTION;
  constant->type = type;
  constant->func = func;
  return constant;
}

lou_sema_const_t *lou_sema_const_new_integer(lou_mempool_t *mempool, lou_sema_type_t *type, uint64_t integer) {
  assert(type->kind == LOU_SEMA_TYPE_INTEGER);
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_INTEGER;
  constant->integer = integer;
  return constant;
}

lou_sema_const_t *lou_sema_const_new_string(lou_mempool_t *mempool, lou_sema_type_t *type, lou_slice_t string) {
  assert(type->kind == LOU_SEMA_TYPE_STRING);
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_STRING;
  constant->type = type;
  constant->string = string;
  return constant;
}

lou_sema_const_t *lou_sema_const_new_extern(lou_mempool_t *mempool, lou_sema_type_t *type, lou_slice_t name) {
  assert(type->kind == LOU_SEMA_TYPE_POINTER);
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_EXTERN;
  constant->type = type;
  constant->external_name = name;
  return constant;
}

