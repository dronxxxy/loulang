#include "utils.h"
#include "emit/type.h"
#include "module.h"
#include <assert.h>
#include <llvm-c/Core.h>

inline char *lou_llvm_opt_slice(lou_llvm_module_t *llvm, lou_opt_slice_t opt_name) {
  char *name = "";
  if (opt_name.has_value) {
    name = lou_mempool_slice_to_cstr(llvm->mempool, opt_name.value);
  }
  return name;
}

LLVMValueRef lou_llvm_extern_decl(lou_llvm_module_t *llvm, lou_slice_t name, lou_hir_type_t *type) {
  if (type->kind == LOU_HIR_TYPE_FUNC) {
    return lou_llvm_function_decl(llvm, (lou_opt_slice_t) { true, name }, type);
  } else {
    return LLVMAddGlobal(llvm->module, lou_llvm_emit_type(llvm, type), lou_mempool_slice_to_cstr(llvm->mempool, name));
  }
}

LLVMValueRef lou_llvm_function_decl(lou_llvm_module_t *llvm, lou_opt_slice_t opt_name, lou_hir_type_t *type) {
  return LLVMAddFunction(llvm->module, lou_llvm_opt_slice(llvm, opt_name), lou_llvm_emit_function_type(llvm, type));
}


