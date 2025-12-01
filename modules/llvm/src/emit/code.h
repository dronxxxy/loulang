#pragma once

#include "lou/hir/func.h"
#include "lou/llvm/module.h"
#include <llvm-c/Types.h>

LLVMBasicBlockRef lou_llvm_emit_code(lou_llvm_module_t *llvm, lou_hir_code_t *code);
