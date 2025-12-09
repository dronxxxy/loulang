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

static inline bool lou_llvm_br(lou_llvm_module_t *llvm, LLVMBasicBlockRef or) {
  LLVMValueRef instr = LLVMGetLastInstruction(LLVMGetInsertBlock(llvm->builder));
  if (!instr || !LLVMIsATerminatorInst(instr)) {
    LLVMBuildBr(llvm->builder, or);
    return true;
  }
  return false;
}

static inline void lou_llvm_emit_stmt(lou_llvm_module_t *llvm, lou_hir_stmt_t *stmt) {
  switch (stmt->kind) {
    case LOU_HIR_STMT_LOOP: {
      LLVMBasicBlockRef begin = LLVMAppendBasicBlockInContext(llvm->context, llvm->function, "");
      LLVMBasicBlockRef end = LLVMAppendBasicBlockInContext(llvm->context, llvm->function, "");

      stmt->loop.codegen.begin = begin;
      stmt->loop.codegen.end = end;

      lou_llvm_emit_code_in(llvm, begin, stmt->cond.code);
      bool has_end = lou_llvm_br(llvm, end);

      LLVMPositionBuilderAtEnd(llvm->builder, end);
      if (!has_end) LLVMBuildUnreachable(llvm->builder);
      return;
    }
    case LOU_HIR_STMT_COND: {
      LLVMValueRef condition = lou_llvm_emit_value(llvm, stmt->cond.condition);
      LLVMBasicBlockRef initial = LLVMGetInsertBlock(llvm->builder);

      LLVMBasicBlockRef then = lou_llvm_emit_code(llvm, stmt->cond.code);
      LLVMBasicBlockRef or_else = stmt->cond.fallback ? lou_llvm_emit_code(llvm, stmt->cond.fallback) : NULL;
      LLVMBasicBlockRef end = LLVMAppendBasicBlockInContext(llvm->context, llvm->function, "");
      bool has_end = !stmt->cond.fallback;

      LLVMPositionBuilderAtEnd(llvm->builder, or_else);
      if (lou_llvm_br(llvm, end)) has_end = true;
      
      LLVMPositionBuilderAtEnd(llvm->builder, then);
      if (lou_llvm_br(llvm, end)) has_end = true;

      LLVMPositionBuilderAtEnd(llvm->builder, initial);
      LLVMBuildCondBr(llvm->builder, condition, then, or_else);

      LLVMPositionBuilderAtEnd(llvm->builder, end);
      if (!has_end) LLVMBuildUnreachable(llvm->builder);
      return;
    }
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
      return;
    case LOU_HIR_STMT_BINOP: {
      LLVMValueRef l = lou_llvm_emit_value(llvm, stmt->binop.left);
      LLVMValueRef r = lou_llvm_emit_value(llvm, stmt->binop.right);
      lou_hir_local_t *output = stmt->binop.output;
      switch (stmt->binop.kind) {
        case LOU_HIR_BINOP_ARITHM:
          switch (stmt->binop.arithm.of) {
            case LOU_HIR_BINOP_ARITHM_OF_INT: {
              assert(output->type->kind == LOU_HIR_TYPE_INT);
              bool is_signed = output->type->integer.is_signed;
              switch (stmt->binop.arithm.kind) {
                case LOU_HIR_BINOP_ARITHM_ADD: lou_llvm_store(llvm, output, LLVMBuildAdd(llvm->builder, l, r, "")); return;
                case LOU_HIR_BINOP_ARITHM_SUBTRACT: lou_llvm_store(llvm, output, LLVMBuildSub(llvm->builder, l, r, "")); return;
                case LOU_HIR_BINOP_ARITHM_MULTIPLY: lou_llvm_store(llvm, output, LLVMBuildMul(llvm->builder, l, r, "")); return;
                case LOU_HIR_BINOP_ARITHM_DIVIDE:
                  lou_llvm_store(llvm, output, (is_signed ? LLVMBuildSDiv : LLVMBuildUDiv)(llvm->builder, l, r, ""));
                  return;
              }
              UNREACHABLE();
            }
            UNREACHABLE();
        }
        case LOU_HIR_BINOP_ARITHM_INT: {
          assert(output->type->kind == LOU_HIR_TYPE_INT);
          bool is_signed = output->type->integer.is_signed;
          switch (stmt->binop.arithm_int) {
            case LOU_HIR_BINOP_ARITHM_INT_MOD:
              lou_llvm_store(llvm, output, (is_signed ? LLVMBuildSRem : LLVMBuildURem)(llvm->builder, l, r, ""));
              return;
          }
          UNREACHABLE();
        }
        case LOU_HIR_BINOP_EQ: {
          switch (stmt->binop.eq.of) {
            case LOU_HIR_BINOP_EQ_OF_BOOL:
            case LOU_HIR_BINOP_EQ_OF_INT:
              switch (stmt->binop.eq.kind) {
                case LOU_HIR_BINOP_EQUALS: lou_llvm_store(llvm, output, LLVMBuildICmp(llvm->builder, LLVMIntEQ, l, r, "")); return;
                case LOU_HIR_BINOP_NOT_EQUALS: lou_llvm_store(llvm, output, LLVMBuildICmp(llvm->builder, LLVMIntNE, l, r, "")); return;
              }
              UNREACHABLE();
          }
          UNREACHABLE();
        }
        case LOU_HIR_BINOP_ORDER:
          switch (stmt->binop.order.of) {
            case LOU_HIR_BINOP_ORDER_OF_INT:
              assert(output->type->kind == LOU_HIR_TYPE_INT);
              bool is_signed = output->type->integer.is_signed;
              switch (stmt->binop.order.kind) {
                case LOU_HIR_BINOP_ORDER_GREATER_OR_EQUALS:
                  lou_llvm_store(llvm, output, LLVMBuildICmp(llvm->builder, is_signed ? LLVMIntSGE : LLVMIntUGE, l, r, ""));
                  return;
                case LOU_HIR_BINOP_ORDER_LESS_OR_EQUALS:
                  lou_llvm_store(llvm, output, LLVMBuildICmp(llvm->builder, is_signed ? LLVMIntSLE : LLVMIntULE, l, r, ""));
                  return;
                case LOU_HIR_BINOP_ORDER_GREATER:
                  lou_llvm_store(llvm, output, LLVMBuildICmp(llvm->builder, is_signed ? LLVMIntSGT : LLVMIntUGT, l, r, ""));
                  return;
                case LOU_HIR_BINOP_ORDER_LESS:
                  lou_llvm_store(llvm, output, LLVMBuildICmp(llvm->builder, is_signed ? LLVMIntSLT : LLVMIntULT, l, r, ""));
                  return;
              }
              UNREACHABLE();
          }
          UNREACHABLE();
      }
      UNREACHABLE();
    }
    case LOU_HIR_STMT_BREAK: LLVMBuildBr(llvm->builder, stmt->break_loop->loop.codegen.end); return;
    case LOU_HIR_STMT_CONTINUE: LLVMBuildBr(llvm->builder, stmt->break_loop->loop.codegen.begin); return;
  }
  UNREACHABLE();
}

inline void lou_llvm_emit_code_in(lou_llvm_module_t *llvm, LLVMBasicBlockRef block, lou_hir_code_t *code) {
  LLVMPositionBuilderAtEnd(llvm->builder, block);
  // TODO: fix stack leak in loops
  for (size_t i = 0; i < lou_vec_len(code->locals); i++) {
    lou_hir_local_t *local = code->locals[i];
    switch (local->mutability) {
      case LOU_HIR_MUTABLE: local->codegen = LLVMBuildAlloca(llvm->builder, lou_llvm_emit_type(llvm, local->type), ""); break;
      case LOU_HIR_IMMUTABLE: break;
    }
  }
  for (size_t i = 0; i < lou_vec_len(code->stmts); i++) {
    lou_llvm_emit_stmt(llvm, code->stmts[i]);
  }
}

LLVMBasicBlockRef lou_llvm_emit_code(lou_llvm_module_t *llvm, lou_hir_code_t *code) {
  LLVMBasicBlockRef block = LLVMAppendBasicBlock(llvm->function, "");
  lou_llvm_emit_code_in(llvm, block, code);
  return block;
}

