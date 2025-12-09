#include "log.h"
#include "lou/core/exec.h"
#include "lou/core/log.h"
#include "lou/core/slice.h"
#include "lou/sema/sema.h"
#include "lou/llvm/module.h"
#include <assert.h>

#define OUTPUT "build/out"
#define OBJ_OUTPUT OUTPUT ".o"

int main(int argc, char** argv) {
  assert(argc == 2);
  log_init();

  lou_sema_t *sema = lou_sema_new(lou_slice_from_cstr(argv[1]));
  if (!sema) return 1;
  lou_sema_read(sema);
  lou_sema_analyze(sema);
  if (lou_sema_failed(sema)) {
    lou_sema_free(sema);
    return 1;
  }

  lou_llvm_module_t *llvm = lou_llvm_module_new(lou_sema_hir(sema));
  lou_llvm_module_emit(llvm);
  lou_llvm_module_dump(llvm, OBJ_OUTPUT);
  lou_llvm_module_free(llvm);
  lou_sema_free(sema);

  char *const args[] = {
    OBJ_OUTPUT,
    "-o", OUTPUT,
  };
  int exec_status;
  if (!lou_exec("/usr/bin/gcc", args, sizeof(args) / sizeof(args[0]), &exec_status) || exec_status != 0) {
    lou_log_fmt(LOG_ERROR, "linking failed with the error");
    return 1;
  }

  if (!lou_exec(OUTPUT, NULL, 0, &exec_status)) {
    return 1;
  }

  if (exec_status != 0) {
    lou_log_fmt(LOG_ERROR, "running failed with status #i", exec_status);
  }

  return exec_status;
}
