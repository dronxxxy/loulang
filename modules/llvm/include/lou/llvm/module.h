#pragma once

#include "lou/hir/hir.h"
#include <stdbool.h>

typedef struct lou_llvm_module_t lou_llvm_module_t;

lou_llvm_module_t *lou_llvm_module_new(lou_hir_t *hir);
void lou_llvm_module_emit(lou_llvm_module_t *llvm);
bool lou_llvm_module_dump(lou_llvm_module_t *llvm, const char *output);
void lou_llvm_module_free(lou_llvm_module_t *llvm);
