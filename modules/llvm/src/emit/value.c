#include "value.h"
#include "emit/const.h"
#include "emit/type.h"
#include "lou/core/assertions.h"
#include "lou/hir/decl.h"
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

LLVMValueRef lou_llvm_emit_value(lou_llvm_module_t *llvm, lou_hir_value_t *value) {
  switch (value->kind) {
    case LOU_HIR_VALUE_CONST:
      return lou_llvm_emit_const(llvm, value->constant);
    case LOU_HIR_VALUE_LOCAL:
      return value->local->codegen;
    case LOU_HIR_VALUE_DECL:
      return LLVMBuildLoad2(llvm->builder, lou_llvm_emit_type(llvm, value->decl->type), value->decl->codegen, "");
  }
  UNREACHABLE();
}
