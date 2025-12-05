#include "lou/hir/const.h"
#include "extern.h"
#include "lou/hir/func.h"
#include "lou/hir/type.h"

lou_hir_const_t *lou_hir_const_new_integer(lou_mempool_t *mempool, lou_hir_type_t *type, uint64_t integer) {
  lou_hir_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_hir_const_t);
  constant->kind = LOU_HIR_CONST_INTEGER;
  constant->type = type;
  constant->integer.value = integer;
  return constant;
}

lou_hir_const_t *lou_hir_const_new_string(lou_mempool_t *mempool, lou_slice_t content, bool null_terminated) {
  lou_hir_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_hir_const_t);
  constant->kind = LOU_HIR_CONST_STRING;
  constant->type = lou_hir_type_new_pointer(mempool, lou_hir_type_new_integer(mempool, LOU_HIR_INT_8, false));
  constant->string.content = content;
  constant->string.null_terminated = null_terminated;
  return constant;
}

lou_hir_const_t *lou_hir_const_new_func(lou_mempool_t *mempool, lou_hir_func_t *func) {
  lou_hir_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_hir_const_t);
  constant->kind = LOU_HIR_CONST_FUNCTION;
  constant->type = func->type;
  constant->func = func;
  return constant;
}

lou_hir_const_t *lou_hir_const_new_extern(lou_mempool_t *mempool, lou_hir_extern_t *external) {
  lou_hir_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_hir_const_t);
  constant->kind = LOU_HIR_CONST_EXTERNAL;
  constant->type = external->type;
  constant->external = external;
  return constant;
}

