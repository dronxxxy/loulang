#include "log.h"
#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/core/vec.h"
#include "lou/hir/code.h"
#include "lou/hir/const.h"
#include "lou/hir/decl.h"
#include "lou/hir/func.h"
#include "lou/hir/type.h"
#include "lou/hir/value.h"
#include "lou/sema/sema.h"
#include "lou/hir/hir.h"
#include "lou/llvm/module.h"
#include <string.h>

void call_putchar(lou_mempool_t *mempool, lou_hir_code_t *code, lou_hir_decl_t *put_char, char c) {
  lou_hir_type_t *type_u8 = lou_hir_type_new_integer(mempool, LOU_HIR_INT_8, false);
  lou_hir_code_append_stmt(code, lou_hir_stmt_new_call(mempool, NULL, lou_hir_value_new_decl(mempool, put_char), ({
    lou_hir_value_t **args = LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_value_t*);
    *LOU_VEC_PUSH(&args) = lou_hir_value_new_const(mempool, lou_hir_const_new_integer(mempool, type_u8, c));
    args;
  })));
}

lou_hir_t *build_hir(lou_mempool_t *mempool) {
  lou_hir_type_t *type_u8 = lou_hir_type_new_integer(mempool, LOU_HIR_INT_8, false);
  lou_hir_type_t *type_i32 = lou_hir_type_new_integer(mempool, LOU_HIR_INT_8, false);

  lou_hir_t *hir = lou_hir_new(mempool);
  lou_hir_decl_t *put_char = lou_hir_decl_new(mempool, LOU_HIR_IMMUTABLE, lou_hir_type_new_func(mempool, ({
    lou_hir_type_t **args = LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_type_t*);
    *LOU_VEC_PUSH(&args) = type_u8;
    args;
  }), NULL));
  lou_hir_decl_add(hir, put_char);
  lou_hir_decl_init(put_char, lou_hir_const_new_extern(mempool, put_char->type, lou_slice_from_cstr("putchar")));

  lou_hir_decl_t *main = lou_hir_decl_new(mempool, LOU_HIR_IMMUTABLE, lou_hir_type_new_func(mempool, ({
    lou_hir_type_t **args = LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_type_t*);
    *LOU_VEC_PUSH(&args) = type_i32;
    args;
  }), NULL));
  lou_hir_decl_add(hir, main);

  lou_hir_func_t *main_func = lou_hir_func_new(mempool);
  lou_hir_func_set_global(main_func, lou_slice_from_cstr("main"));

  lou_hir_code_t *main_code = lou_hir_code_new(mempool);
  const char *message = "hello, world!\n";
  for (size_t i = 0; i < strlen(message); i++) {
    call_putchar(mempool, main_code, put_char, message[i]);
  }
  lou_hir_code_append_stmt(main_code, lou_hir_stmt_new_ret(mempool, lou_hir_value_new_const(mempool, lou_hir_const_new_integer(mempool, type_i32, 0))));
  lou_hir_decl_init(main, lou_hir_const_new_func(mempool,
      lou_hir_type_new_func(mempool, LOU_MEMPOOL_VEC_NEW(mempool, lou_hir_type_t*), type_i32),
      main_func
    )
  );
  lou_hir_func_init(main_func, main_code);
  return hir;
}

int main(int argc, char** argv) {
  log_init();


  lou_sema_t *sema = lou_sema_new(lou_slice_from_cstr("./examples/test/lexer.lou"));
  if (!sema) return 1;
  lou_sema_read(sema);
  lou_sema_analyze(sema);
  lou_llvm_module_t *llvm = lou_llvm_module_new(lou_sema_hir(sema));
  lou_llvm_module_emit(llvm);
  lou_llvm_module_dump(llvm, "build/out");
  lou_llvm_module_free(llvm);

  int status = lou_sema_failed(sema) ? 1 : 0;
  lou_sema_free(sema);
  return status;
}
