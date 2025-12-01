#pragma once

#include "lou/llvm/module.h"
#include "lou/hir/decl.h"
#include <llvm-c/Types.h>

LLVMValueRef lou_llvm_init_decl(lou_llvm_module_t *llvm, const lou_hir_decl_t *decl);
void lou_llvm_emit_decl(lou_llvm_module_t *llvm, const lou_hir_decl_t *decl);
