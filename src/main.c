#include "log.h"
#include "lou/core/slice.h"
#include "lou/sema/sema.h"
#include "lou/llvm/module.h"

int main(int argc, char** argv) {
  log_init();

  lou_sema_t *sema = lou_sema_new(lou_slice_from_cstr("./examples/test/lexer.lou"));
  if (!sema) return 1;
  lou_sema_read(sema);
  lou_sema_analyze(sema);
  if (lou_sema_failed(sema)) {
    lou_sema_free(sema);
    return 1;
  }
  lou_llvm_module_t *llvm = lou_llvm_module_new(lou_sema_hir(sema));
  lou_llvm_module_emit(llvm);
  lou_llvm_module_dump(llvm, "build/out");
  lou_llvm_module_free(llvm);

  int status = lou_sema_failed(sema) ? 1 : 0;
  lou_sema_free(sema);
  return status;
}
