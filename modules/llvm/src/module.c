#include "lou/llvm/module.h"
#include "emit/decl.h"
#include <assert.h>
#include "lou/core/log.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/hir/hir.h"
#include "module.h"
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
  return llvm;
}

void lou_llvm_module_emit(lou_llvm_module_t *llvm) {
  lou_hir_decl_t **decls = lou_hir_get_decls(llvm->hir);
  for (size_t i = 0; i < lou_vec_len(decls); i++) {
    decls[i]->codegen = lou_llvm_init_decl(llvm, decls[i]);
  }
  for (size_t i = 0; i < lou_vec_len(decls); i++) {
    lou_llvm_emit_decl(llvm, decls[i]);
  }
}

bool lou_llvm_module_dump(lou_llvm_module_t *llvm, const char *output) {
  char *error = NULL;
  LLVMTargetMachineEmitToFile(llvm->machine, llvm->module, output, LLVMObjectFile, &error);
  if (error) {
    lou_log_fmt(LOG_ERROR, "failed to emit file: \n#s", error);
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
