#include "decl.h"
#include "../module.h"
#include "emit/const.h"
#include "lou/core/assertions.h"
#include "type.h"
#include <llvm-c/Core.h>

LLVMValueRef lou_llvm_init_decl(lou_llvm_module_t *llvm, const lou_hir_decl_t *decl) {
  switch (decl->mutability) {
    case LOU_HIR_MUTABLE:
      return LLVMAddGlobal(llvm->module, lou_llvm_emit_type(llvm, decl->type), "");
    case LOU_HIR_IMMUTABLE:
      return lou_llvm_init_const(llvm, decl->initializer);
  }
  UNREACHABLE();
}

void lou_llvm_emit_decl(lou_llvm_module_t *llvm, const lou_hir_decl_t *decl) {
  switch (decl->mutability) {
    case LOU_HIR_MUTABLE:
      LLVMSetInitializer(decl->codegen, decl->initializer ?
        lou_llvm_emit_const(llvm, decl->initializer) :
        LLVMGetUndef(lou_llvm_emit_type(llvm, decl->type))
      );
      return;
    case LOU_HIR_IMMUTABLE:
      return lou_llvm_finilize_const(llvm, decl->codegen, decl->initializer);
  }
  UNREACHABLE();
}
