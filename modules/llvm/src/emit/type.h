#pragma once

#include "lou/hir/type.h"
#include "module.h"

LLVMTypeRef lou_llvm_emit_type(lou_llvm_module_t *llvm, lou_hir_type_t *type);
LLVMTypeRef lou_llvm_emit_function_type(lou_llvm_module_t *llvm, lou_hir_type_t *type);
