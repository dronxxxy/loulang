#include "const.h"
#include "lou/core/assertions.h"
#include "lou/hir/const.h"
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

lou_sema_const_t *lou_sema_const_new_extern(lou_mempool_t *mempool, lou_sema_type_t *type, lou_hir_extern_t *external) {
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_EXTERN;
  constant->type = type;
  constant->external = external;
  return constant;
}

lou_sema_const_t *lou_sema_const_new_string(lou_mempool_t *mempool, lou_slice_t content) {
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_STRING;
  constant->type = lou_sema_type_new_string(mempool);
  constant->string = content;
  return constant;
}

lou_sema_const_t *lou_sema_const_new_cstring(lou_mempool_t *mempool, lou_slice_t content) {
  lou_sema_const_t *constant = LOU_MEMPOOL_ALLOC(mempool, lou_sema_const_t);
  constant->kind = LOU_SEMA_CONST_CSTRING;
  constant->type = lou_sema_type_new_pointer(mempool);
  lou_sema_type_init_pointer(constant->type, lou_sema_type_new_int(mempool, LOU_SEMA_INT_8, false));
  constant->cstring = content;
  return constant;
}

lou_hir_const_t *lou_sema_const_as_hir(lou_mempool_t *mempool, lou_sema_const_t *constant) {
  lou_hir_type_t *type = lou_sema_type_as_hir(mempool, constant->type);
  switch (constant->kind) {
    case LOU_SEMA_CONST_INTEGER: return lou_hir_const_new_integer(mempool, type, constant->integer);
    case LOU_SEMA_CONST_FUNC: return lou_hir_const_new_func(mempool, constant->func);
    case LOU_SEMA_CONST_STRING: NOT_IMPLEMENTED;
    case LOU_SEMA_CONST_EXTERN: return lou_hir_const_new_extern(mempool, constant->external);
    case LOU_SEMA_CONST_CSTRING: return lou_hir_const_new_string(mempool, constant->cstring, true);
  }
  UNREACHABLE();
}
