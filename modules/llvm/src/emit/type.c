#include "type.h"
#include "lou/core/assertions.h"
#include "lou/core/vec.h"
#include <llvm-c/Core.h>

LLVMTypeRef lou_llvm_emit_type(lou_llvm_module_t *llvm, lou_hir_type_t *type) {
  if (!type) {
    return LLVMVoidType();
  }
  switch (type->kind) {
    case LOU_HIR_TYPE_INT:
      switch (type->integer.size) {
        case LOU_HIR_INT_8: return LLVMInt8TypeInContext(llvm->context);
        case LOU_HIR_INT_16: return LLVMInt16TypeInContext(llvm->context);
        case LOU_HIR_INT_32: return LLVMInt32TypeInContext(llvm->context);
        case LOU_HIR_INT_64: return LLVMInt64TypeInContext(llvm->context);
      }
      UNREACHABLE();
    case LOU_HIR_TYPE_POINTER: case LOU_HIR_TYPE_FUNC: return LLVMPointerTypeInContext(llvm->context, 0);
    case LOU_HIR_TYPE_BOOL: return LLVMInt1TypeInContext(llvm->context);
  }
  UNREACHABLE();
}

LLVMTypeRef lou_llvm_emit_function_type(lou_llvm_module_t *llvm, lou_hir_type_t *type) {
  assert(type->kind == LOU_HIR_TYPE_FUNC);
  LLVMTypeRef *types = LOU_MEMPOOL_VEC_NEW(llvm->mempool, LLVMTypeRef);
  LOU_VEC_RESERVE(&types, lou_vec_len(type->func.args));
  for (size_t i = 0; i < lou_vec_len(type->func.args); i++) {
    *LOU_VEC_PUSH(&types) = lou_llvm_emit_type(llvm, type->func.args[i]);
  }
  return LLVMFunctionType(lou_llvm_emit_type(llvm, type->func.returns), types, lou_vec_len(types), false);
}
