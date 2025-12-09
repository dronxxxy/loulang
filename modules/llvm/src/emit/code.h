#pragma once

#include "lou/hir/func.h"
#include "lou/llvm/module.h"
#include <llvm-c/Types.h>

void lou_llvm_emit_code_in(lou_llvm_module_t *llvm, LLVMBasicBlockRef block, lou_hir_code_t *code);
LLVMBasicBlockRef lou_llvm_emit_code(lou_llvm_module_t *llvm, lou_hir_code_t *code);
