#include "code.h"
#include "emit/type.h"
#include "emit/value.h"
#include "lou/core/assertions.h"
#include "lou/core/vec.h"
#include "lou/hir/code.h"
#include "lou/hir/value.h"
#include "module.h"
#include <alloca.h>
#include <llvm-c/Core.h>
#include <llvm-c/Types.h>

static inline void lou_llvm_emit_stmt(lou_llvm_module_t *llvm, lou_hir_stmt_t *stmt) {
  switch (stmt->kind) {
    case LOU_HIR_STMT_ARG:
      lou_llvm_store(llvm, stmt->arg.output, LLVMGetParam(llvm->function, stmt->arg.num));
      return;
    case LOU_HIR_STMT_CALL: {
      LLVMTypeRef type = lou_llvm_emit_function_type(llvm, lou_hir_value_typeof(stmt->call.callable));
      LLVMValueRef *args = alloca(sizeof(LLVMValueRef) * lou_vec_len(stmt->call.args));
      for (size_t i = 0; i < lou_vec_len(stmt->call.args); i++) {
        args[i] = lou_llvm_emit_value(llvm, stmt->call.args[i]);
      }
      lou_llvm_store(llvm,
        stmt->call.output,
        LLVMBuildCall2(llvm->builder, type, lou_llvm_emit_value(llvm, stmt->call.callable), args, lou_vec_len(stmt->call.args), "")
      );
      return;
    }
    case LOU_HIR_STMT_RETURN:
      LLVMBuildRet(llvm->builder, lou_llvm_emit_value(llvm, stmt->ret.value));
      return;
    case LOU_HIR_STMT_CODE:
      lou_llvm_emit_code(llvm, stmt->code);
      return;
  }
  UNREACHABLE();
}

LLVMBasicBlockRef lou_llvm_emit_code(lou_llvm_module_t *llvm, lou_hir_code_t *code) {
  LLVMBasicBlockRef block = LLVMAppendBasicBlock(llvm->function, "");
  LLVMPositionBuilderAtEnd(llvm->builder, block);
  for (size_t i = 0; i < lou_vec_len(code->stmts); i++) {
    lou_llvm_emit_stmt(llvm, code->stmts[i]);
  }
  return block;
}

