#include "lou/llvm/module.h"
#include <assert.h>
#include "emit/code.h"
#include "lou/core/log.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/hir/extern.h"
#include "lou/hir/func.h"
#include "lou/hir/hir.h"
#include "module.h"
#include "utils.h"
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Core.h>

static inline LLVMTargetMachineRef llvm_default_target_machine() {
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmParser();
  LLVMInitializeNativeAsmPrinter();

  char *cpu = LLVMGetHostCPUName();
  char *features = LLVMGetHostCPUFeatures();
  char *triple = LLVMGetDefaultTargetTriple();
  assert(cpu);
  assert(features);
  assert(triple);

  LLVMTargetRef target = LLVMGetFirstTarget();
  assert(target);
  LLVMTargetMachineRef machine = LLVMCreateTargetMachine(
    target,
    triple,
    cpu,
    features,
    LLVMCodeGenLevelNone,
    LLVMRelocDefault,
    LLVMCodeModelDefault
  );
  assert(machine);
  LLVMDisposeMessage(cpu);
  LLVMDisposeMessage(features);
  LLVMDisposeMessage(triple);
  return machine;
}

lou_llvm_module_t *lou_llvm_module_new(lou_hir_t *hir) {
  lou_mempool_t *mempool = lou_mempool_new();
  lou_llvm_module_t *llvm = LOU_MEMPOOL_ALLOC(mempool, lou_llvm_module_t);
  llvm->mempool = mempool;
  llvm->function = NULL;
  llvm->hir = hir;
  llvm->context = LLVMContextCreate();
  llvm->machine = llvm_default_target_machine();
  llvm->module = LLVMModuleCreateWithNameInContext("main", llvm->context);
  llvm->builder = LLVMCreateBuilderInContext(llvm->context);
  llvm->loop_vars = NULL;
  return llvm;
}

static inline void lou_llvm_emit_function(lou_llvm_module_t *llvm, lou_hir_func_t *func) {
  assert(!llvm->function);
  llvm->function = func->codegen;
  lou_llvm_emit_code(llvm, func->code);
  llvm->function = NULL;
}

void lou_llvm_module_emit(lou_llvm_module_t *llvm) {
  for (size_t i = 0; i < lou_vec_len(llvm->hir->functions); i++) {
    lou_hir_func_t *func = llvm->hir->functions[i];
    func->codegen = lou_llvm_function_decl(llvm, func->name, func->type);
  }
  for (size_t i = 0; i < lou_vec_len(llvm->hir->externs); i++) {
    lou_hir_extern_t *external = llvm->hir->externs[i];
    bool skip = false;
    for (size_t j = 0; j < i; j++) {
      lou_hir_extern_t *prev_external = llvm->hir->externs[j];
      if (lou_slice_eq(external->name, prev_external->name)) {
        external->codegen = prev_external->codegen;
        skip = true;
        break;
      }
    }
    if (skip) continue;
    external->codegen = lou_llvm_extern_decl(llvm, external->name, external->type);
  }

  for (size_t i = 0; i < lou_vec_len(llvm->hir->functions); i++) {
    lou_llvm_emit_function(llvm, llvm->hir->functions[i]);
  }
}

bool lou_llvm_module_print(lou_llvm_module_t *llvm, const char *output) {
  char *error = NULL;
  LLVMPrintModuleToFile(llvm->module, output, &error);
  if (error) {
    lou_log_fmt(LOU_LOG_ERROR, "failed to print file: \n#s", error);
    return false;
  }
  return true;
}

bool lou_llvm_module_dump(lou_llvm_module_t *llvm, const char *output) {
  char *error = NULL;
  LLVMTargetMachineEmitToFile(llvm->machine, llvm->module, output, LLVMObjectFile, &error);
  if (error) {
    lou_log_fmt(LOU_LOG_ERROR, "failed to emit file: \n#s", error);
    return false;
  }
  return true;
}

void lou_llvm_module_free(lou_llvm_module_t *llvm) {
  LLVMDisposeTargetMachine(llvm->machine);
  LLVMContextDispose(llvm->context);
  LLVMDisposeBuilder(llvm->builder);
  lou_mempool_free(llvm->mempool);
}
