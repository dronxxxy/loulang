#pragma once

#include <llvm-c/TargetMachine.h>
#include "lou/core/mempool.h"
#include "lou/hir/hir.h"

typedef struct lou_llvm_module_t {
  lou_mempool_t *mempool;
  lou_hir_t *hir;
  LLVMValueRef function;
  LLVMTargetMachineRef machine;
  LLVMModuleRef module;
  LLVMContextRef context;
  LLVMBuilderRef builder;
} lou_llvm_module_t;

