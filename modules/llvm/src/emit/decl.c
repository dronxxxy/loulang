#include "decl.h"
#include "../module.h"
#include "emit/const.h"
#include "type.h"
#include <llvm-c/Core.h>

LLVMValueRef lou_llvm_init_decl(lou_llvm_module_t *llvm, const lou_hir_decl_t *decl) {
  return LLVMAddGlobal(llvm->module, lou_llvm_emit_type(llvm, decl->type), "");
}

void lou_llvm_emit_decl(lou_llvm_module_t *llvm, const lou_hir_decl_t *decl) {
  LLVMSetInitializer(decl->codegen, decl->initializer ?
    lou_llvm_emit_const(llvm, decl->initializer) :
    LLVMGetUndef(lou_llvm_emit_type(llvm, decl->type))
  );
}
