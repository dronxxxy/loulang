#include "type.h"

lou_sema_type_t *lou_sema_type_new_string(lou_mempool_t *mempool) {
  lou_sema_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_sema_type_t);
  type->kind = LOU_SEMA_TYPE_STRING;
  return type;
}

lou_sema_type_t *lou_sema_type_new_function(lou_mempool_t *mempool, lou_sema_type_func_t func) {
  lou_sema_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_sema_type_t);
  type->kind = LOU_SEMA_TYPE_FUNCTION;
  type->func = func;
  return type;
}

lou_sema_type_t *lou_sema_type_new_pointer(lou_mempool_t *mempool, lou_sema_type_t *to) {
  lou_sema_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_sema_type_t);
  type->kind = LOU_SEMA_TYPE_POINTER;
  type->pointer_to = to;
  return type;
}

lou_sema_type_t *lou_sema_type_new_integer(lou_mempool_t *mempool, lou_sema_type_int_t integer) {
  lou_sema_type_t *type = LOU_MEMPOOL_ALLOC(mempool, lou_sema_type_t);
  type->kind = LOU_SEMA_TYPE_INTEGER;
  type->integer = integer;
  return type;
}

