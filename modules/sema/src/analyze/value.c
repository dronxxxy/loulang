#include "value.h"
#include "const.h"
#include "lou/core/assertions.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/hir/const.h"
#include "lou/hir/type.h"
#include "lou/hir/value.h"
#include <assert.h>

static inline lou_hir_int_size_t lou_sema_emit_int_size(lou_sema_int_size_t size) {
  switch (size) {
    case LOU_SEMA_INT_8: return LOU_HIR_INT_8;
    case LOU_SEMA_INT_16: return LOU_HIR_INT_16;
    case LOU_SEMA_INT_32: return LOU_HIR_INT_32;
    case LOU_SEMA_INT_64: return LOU_HIR_INT_64;
  }
  UNREACHABLE();
}

lou_hir_type_t *lou_sema_emit_type(lou_mempool_t *mempool, lou_sema_type_t *type) {
  switch (type->kind) {
    case LOU_SEMA_TYPE_INTEGER: return lou_hir_type_new_integer(mempool, lou_sema_emit_int_size(type->integer.size), type->integer.is_signed);
    case LOU_SEMA_TYPE_FUNCTION: {
      lou_hir_type_t **args = LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_type_t*);
      for (size_t i = 0; i < lou_vec_len(type->func.args); i++) {
        *LOU_VEC_PUSH(&args) = lou_sema_emit_type(mempool, type->func.args[i].type);
      }
      lou_hir_type_t *returns = type->func.returns ? lou_sema_emit_type(mempool, type->func.returns) : NULL;
      return lou_hir_type_new_func(mempool, args, returns);
    }
    case LOU_SEMA_TYPE_POINTER: NOT_IMPLEMENTED; // return lou_hir_type_new_pointer(mempool, lou_sema_emit_type(mempool, type->pointer_to));
    case LOU_SEMA_TYPE_STRING: NOT_IMPLEMENTED;
  }
  UNREACHABLE();
}

lou_hir_const_t *lou_sema_emit_const(lou_mempool_t *mempool, lou_sema_const_t *constant) {
  switch (constant->kind) {
    case LOU_SEMA_CONST_INTEGER: return lou_hir_const_new_integer(mempool, lou_sema_emit_type(mempool, constant->type), constant->integer);
    case LOU_SEMA_CONST_STRING: NOT_IMPLEMENTED;
    case LOU_SEMA_CONST_FUNCTION: return lou_hir_const_new_func(mempool, lou_sema_emit_type(mempool, constant->type), constant->func);
    case LOU_SEMA_CONST_EXTERN: return lou_hir_const_new_extern(mempool, lou_sema_emit_type(mempool, constant->type), constant->external_name);
  }
  UNREACHABLE();
}

lou_hir_value_t *lou_sema_emit_value(lou_mempool_t *mempool, lou_sema_value_t *value) {
  assert(value->kind == LOU_SEMA_VALUE_RUNTIME);
  switch (value->runtime.kind) {
    case LOU_SEMA_VALUE_RUNTIME_CONSTANT: return lou_hir_value_new_const(mempool, lou_sema_emit_const(mempool, value->runtime.constant));
    case LOU_SEMA_VALUE_RUNTIME_DECL:
      switch (value->runtime.decl.kind) {
        case LOU_SEMA_VALUE_DECL_GLOBAL: return lou_hir_value_new_decl(mempool, value->runtime.decl.global);
        case LOU_SEMA_VALUE_DECL_LOCAL: return lou_hir_value_new_local(mempool, value->runtime.decl.local);
      }
      break;
  }
  UNREACHABLE();
}
