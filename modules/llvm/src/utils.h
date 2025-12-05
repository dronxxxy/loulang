#pragma once

#include "lou/hir/const.h"
#include "lou/llvm/module.h"
#include <llvm-c/Types.h>

LLVMValueRef lou_llvm_function_decl(lou_llvm_module_t *llvm, lou_opt_slice_t opt_name, lou_hir_type_t *type);
LLVMValueRef lou_llvm_extern_decl(lou_llvm_module_t *llvm, lou_slice_t name, lou_hir_type_t *type);
char *lou_llvm_opt_slice(lou_llvm_module_t *llvm, lou_opt_slice_t opt_name);
