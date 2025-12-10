#pragma once

#include "lou/hir/code.h"
#include "lou/llvm/module.h"
#include <llvm-c/Types.h>

LLVMValueRef lou_llvm_emit_local(lou_llvm_module_t *llvm, lou_hir_local_t *local);
LLVMValueRef lou_llvm_emit_value(lou_llvm_module_t *llvm, lou_hir_value_t *value);
void lou_llvm_store(lou_llvm_module_t *llvm, lou_hir_local_t *local, LLVMValueRef value);
