#include "log.h"
#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/core/vec.h"
#include "lou/ir/code.h"
#include "lou/ir/const.h"
#include "lou/ir/decl.h"
#include "lou/ir/func.h"
#include "lou/ir/type.h"
#include "lou/ir/value.h"
#include "lou/sema/sema.h"
#include "lou/ir/ir.h"

lou_ir_t *build_ir(lou_mempool_t *mempool) {
  lou_ir_type_t *type_u8 = lou_ir_type_new_integer(mempool, (lou_ir_type_int_t) {
    .size = LOU_IR_INT_8,
    .is_signed = false,
  });

  lou_ir_type_t *type_i32 = lou_ir_type_new_integer(mempool, (lou_ir_type_int_t) {
    .size = LOU_IR_INT_8,
    .is_signed = false,
  });

  lou_ir_t *ir = lou_ir_new(mempool);
  lou_ir_decl_t *put_char = lou_ir_decl_new(mempool, LOU_IR_MUTABLE, lou_ir_type_new_func(mempool,
    (lou_ir_type_func_t) {
      .args = ({
        lou_ir_type_t **args = LOU_MEMPOOL_VEC_NEW(mempool, lou_ir_type_t*);
        *LOU_VEC_PUSH(&args) = type_u8;
        args;
      }),
      .returns = NULL,
    })
  );
  lou_ir_decl_add(ir, put_char);
  lou_ir_decl_init_extern(put_char, (lou_ir_decl_extern_t) {
    .name = lou_slice_from_cstr("putchar"),
  });

  lou_ir_decl_t *main = lou_ir_decl_new(mempool, LOU_IR_MUTABLE, lou_ir_type_new_func(mempool, (lou_ir_type_func_t) {
    .args = ({
      lou_ir_type_t **args = LOU_MEMPOOL_VEC_NEW(mempool, lou_ir_type_t*);
      *LOU_VEC_PUSH(&args) = type_i32;
      args;
    }),
    .returns = NULL,
  }));
  lou_ir_decl_add(ir, main);
  lou_ir_func_t *main_func = lou_ir_func_new(mempool);
  lou_ir_func_init(main_func, lou_ir_code_new(mempool, ({
    lou_ir_stmt_t **stmts = LOU_MEMPOOL_VEC_NEW(mempool, lou_ir_stmt_t*);
    *LOU_VEC_PUSH(&stmts) = lou_ir_stmt_new_call(mempool, (lou_ir_stmt_call_t) {
      .output = NULL,
      .callable = lou_ir_value_new_decl(mempool, put_char),
      .args = ({
        lou_ir_value_t **args = LOU_MEMPOOL_VEC_NEW(mempool, lou_ir_value_t*);
        *LOU_VEC_PUSH(&args) = lou_ir_value_new_const(mempool, lou_ir_const_new_integer(mempool, type_u8, (lou_ir_const_int_t) { 97 }));
        args;
      }),
    });
    *LOU_VEC_PUSH(&stmts) = lou_ir_stmt_new_ret(mempool, (lou_ir_stmt_ret_t) {
      .value = lou_ir_value_new_const(mempool, lou_ir_const_new_integer(mempool, type_i32, (lou_ir_const_int_t) { 0 })),
    });
    stmts;
  })));
  lou_ir_decl_init_global(main, (lou_ir_decl_global_t) {
    .name = {
      .has_value = true,
      .value = lou_slice_from_cstr("main"),
    },
    .initializer = lou_ir_const_new_func(mempool,
      lou_ir_type_new_func(mempool, (lou_ir_type_func_t) {
        .args = LOU_MEMPOOL_VEC_NEW(mempool, lou_ir_type_t*),
        .returns = type_i32,
      }),
      main_func
    )
  });
  return ir;
}

int main(int argc, char** argv) {
  log_init();

  lou_mempool_t *mempool = lou_mempool_new();
  build_ir(mempool);
  lou_mempool_free(mempool);

  lou_sema_t *sema = lou_sema_new(lou_slice_from_cstr("./examples/test/lexer.lou"));
  if (!sema) return 1;
  lou_sema_read(sema);
  lou_sema_analyze(sema);

  int status = lou_sema_failed(sema) ? 1 : 0;
  lou_sema_free(sema);
  return status;
}
