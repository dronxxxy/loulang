#include "value.h"
#include "emit/const.h"
#include "emit/type.h"
#include "lou/core/assertions.h"
#include "lou/hir/const.h"
#include "lou/hir/func.h"
#include "lou/hir/value.h"
#include "module.h"
#include <llvm-c/Core.h>

void lou_llvm_store(lou_llvm_module_t *llvm, lou_hir_local_t *local, LLVMValueRef value) {
  if (!local) {
    return;
  }
  switch (local->mutability) {
    case LOU_HIR_MUTABLE:
      LLVMBuildStore(llvm->builder, value, local->codegen);
      return;
    case LOU_HIR_IMMUTABLE:
      local->codegen = value;
      return;
  }
  UNREACHABLE();
}

LLVMValueRef llvm_extract_from_mutablity(lou_llvm_module_t *llvm, lou_hir_type_t *type, LLVMValueRef value, lou_hir_mutability_t mutability) {
  switch (mutability) {
    case LOU_HIR_MUTABLE: return LLVMBuildLoad2(llvm->builder, lou_llvm_emit_type(llvm, type), value, "");
    case LOU_HIR_IMMUTABLE: return value;
  }
  UNREACHABLE();
}

LLVMValueRef lou_llvm_emit_value(lou_llvm_module_t *llvm, lou_hir_value_t *value) {
  switch (value->kind) {
    case LOU_HIR_VALUE_CONST: return lou_llvm_emit_const(llvm, value->constant);
    case LOU_HIR_VALUE_LOCAL:
      return llvm_extract_from_mutablity(llvm,
        value->local->type,
        value->local->codegen,
        value->local->mutability
      );
    case LOU_HIR_VALUE_FUNCTION: return value->func->codegen;
  }
  UNREACHABLE();
}
