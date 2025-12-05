#pragma once

#include "lou/hir/const.h"
#include "lou/llvm/module.h"
#include <llvm-c/Types.h>

LLVMValueRef lou_llvm_emit_const(lou_llvm_module_t *llvm, const lou_hir_const_t *constant);

