#include "code.h"
#include "emit/type.h"
#include "emit/value.h"
#include "lou/core/assertions.h"
#include "lou/core/vec.h"
#include "lou/hir/code.h"
#include "lou/hir/value.h"
#include "module.h"
#include <alloca.h>
#include <assert.h>
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
    case LOU_HIR_STMT_STORE_VAR:
      assert(stmt->store_var.output->mutability == LOU_HIR_MUTABLE);
      LLVMBuildStore(llvm->builder, lou_llvm_emit_value(llvm, stmt->store_var.value), stmt->store_var.output->codegen);
      break;
    case LOU_HIR_STMT_BINOP: {
      LLVMValueRef l = lou_llvm_emit_value(llvm, stmt->binop.left);
      LLVMValueRef r = lou_llvm_emit_value(llvm, stmt->binop.right);
      lou_hir_local_t *output = stmt->binop.output;
      switch (stmt->binop.kind) {
        case LOU_HIR_BINOP_ARITHM: {
          assert(output->type->kind == LOU_HIR_TYPE_INT);
          bool is_signed = output->type->integer.is_signed;
          switch (stmt->binop.arithm) {
            case LOU_HIR_BINOP_ARITHM_ADD: lou_llvm_store(llvm, output, LLVMBuildAdd(llvm->builder, l, r, "")); break;
            case LOU_HIR_BINOP_ARITHM_SUBTRACT: lou_llvm_store(llvm, output, LLVMBuildSub(llvm->builder, l, r, "")); break;
            case LOU_HIR_BINOP_ARITHM_MULTIPLY: lou_llvm_store(llvm, output, LLVMBuildMul(llvm->builder, l, r, "")); break;
            case LOU_HIR_BINOP_ARITHM_DIVIDE:
              lou_llvm_store(llvm, output, (is_signed ? LLVMBuildSDiv : LLVMBuildUDiv)(llvm->builder, l, r, ""));
              break;
          }
        }
        case LOU_HIR_BINOP_ARITHM_INT: {
          assert(output->type->kind == LOU_HIR_TYPE_INT);
          bool is_signed = output->type->integer.is_signed;
          switch (stmt->binop.arithm_int) {
            case LOU_HIR_BINOP_ARITHM_INT_MOD:
              lou_llvm_store(llvm, output, (is_signed ? LLVMBuildSRem : LLVMBuildURem)(llvm->builder, l, r, ""));
              break;
          }
        }
      }
      break;
    }
  }
  UNREACHABLE();
}

LLVMBasicBlockRef lou_llvm_emit_code(lou_llvm_module_t *llvm, lou_hir_code_t *code) {
  LLVMBasicBlockRef block = LLVMAppendBasicBlock(llvm->function, "");
  LLVMPositionBuilderAtEnd(llvm->builder, block);
  // TODO: fix stack leak in loops
  for (size_t i = 0; i < lou_vec_len(code->locals); i++) {
    lou_hir_local_t *local = code->locals[i];
    switch (local->mutability) {
      case LOU_HIR_MUTABLE:
        local->codegen = LLVMBuildAlloca(llvm->builder, lou_llvm_emit_type(llvm, local->type), "");
        break;
      case LOU_HIR_IMMUTABLE: break;
    }
  }
  for (size_t i = 0; i < lou_vec_len(code->stmts); i++) {
    lou_llvm_emit_stmt(llvm, code->stmts[i]);
  }
  return block;
}

