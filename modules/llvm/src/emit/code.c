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
      LLVMBasicBlockRef loop_setup = LLVMGetInsertBlock(llvm->builder);
      LLVMBasicBlockRef begin = LLVMAppendBasicBlockInContext(llvm->context, llvm->function, "loop.begin");
      LLVMBasicBlockRef end = LLVMAppendBasicBlockInContext(llvm->context, llvm->function, "loop.end");

      stmt->loop.codegen.begin = begin;
      stmt->loop.codegen.end = end;

      bool is_root_loop = !llvm->loop_vars;
      if (is_root_loop) llvm->loop_vars = loop_setup;
      lou_llvm_emit_code_in(llvm, begin, stmt->loop.code);
      bool has_end = lou_llvm_br(llvm, begin);
      if (is_root_loop) llvm->loop_vars = NULL;

      LLVMPositionBuilderAtEnd(llvm->builder, loop_setup);
      LLVMBuildBr(llvm->builder, begin);

      LLVMPositionBuilderAtEnd(llvm->builder, end);
      if (!has_end) LLVMBuildUnreachable(llvm->builder);
      return;
    }
    case LOU_HIR_STMT_COND: {
      LLVMValueRef condition = lou_llvm_emit_value(llvm, stmt->cond.condition);
      LLVMBasicBlockRef initial = LLVMGetInsertBlock(llvm->builder);

      LLVMBasicBlockRef then = lou_llvm_emit_code(llvm, stmt->cond.code);
      LLVMBasicBlockRef then_end = LLVMGetInsertBlock(llvm->builder);

      LLVMBasicBlockRef end = LLVMAppendBasicBlockInContext(llvm->context, llvm->function, "if.after");

      LLVMBasicBlockRef or_else = stmt->cond.fallback ? lou_llvm_emit_code(llvm, stmt->cond.fallback) : end;
      LLVMBasicBlockRef or_else_end = LLVMGetInsertBlock(llvm->builder);

      bool has_end = stmt->cond.fallback == NULL;

      if (stmt->cond.fallback) {
        LLVMPositionBuilderAtEnd(llvm->builder, or_else_end);
        if (lou_llvm_br(llvm, end)) has_end = true;
      }
      
      LLVMPositionBuilderAtEnd(llvm->builder, then_end);
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
      if (stmt->ret.value) {
        LLVMBuildRet(llvm->builder, lou_llvm_emit_value(llvm, stmt->ret.value));
      } else {
        LLVMBuildRetVoid(llvm->builder);
      }
      return;
    case LOU_HIR_STMT_CODE:
      lou_llvm_emit_code_in(llvm, LLVMGetInsertBlock(llvm->builder), stmt->code);
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
              bool is_signed = lou_hir_value_typeof(stmt->binop.left)->integer.is_signed;
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
    case LOU_HIR_STMT_CAST_INT: {
      lou_hir_type_t *source = lou_hir_value_typeof(stmt->cast_int.value);
      lou_hir_type_t *to = stmt->cast_int.to;
      LLVMValueRef value = lou_llvm_emit_value(llvm, stmt->cast_int.value);
      LLVMTypeRef type = lou_llvm_emit_type(llvm, to);
      if (to->integer.size <= source->integer.size) {
        value = LLVMBuildTruncOrBitCast(llvm->builder, value, type, "");
      } else if (to->integer.is_signed) {
        value = LLVMBuildSExt(llvm->builder, value, type, "");
      } else {
        value = LLVMBuildZExt(llvm->builder, value, type, "");
      }
      lou_llvm_store(llvm, stmt->cast_int.output, value);
      return;
    }
    case LOU_HIR_STMT_NEGATIVE_INT:
      lou_llvm_store(llvm, stmt->negative_int.output, LLVMBuildNeg(llvm->builder, lou_llvm_emit_value(llvm, stmt->negative_int.value), ""));
      return;
    case LOU_HIR_STMT_SET_PSEUDO_VAR:
      assert(stmt->set_pseudo_var.output->pseudo);
      assert(lou_hir_value_typeof(stmt->set_pseudo_var.value)->kind == LOU_HIR_TYPE_POINTER);
      stmt->set_pseudo_var.output->codegen = lou_llvm_emit_value(llvm, stmt->set_pseudo_var.value);
      return;
    case LOU_HIR_STMT_IDX_ARRAY:
      lou_llvm_store(llvm, stmt->idx_array.output, LLVMBuildExtractElement(
        llvm->builder,
        lou_llvm_emit_value(llvm, stmt->idx_array.value),
        lou_llvm_emit_value(llvm, stmt->idx_array.array),
        ""
      ));
      return;
    case LOU_HIR_STMT_IDX_VAR_ARRAY: {
      LLVMValueRef idx = lou_llvm_emit_value(llvm, stmt->idx_var_array.value);
      assert(stmt->idx_var_array.array->mutability == LOU_HIR_MUTABLE);
      LLVMValueRef value = stmt->idx_var_array.array->codegen;
      LLVMTypeRef array_type = lou_llvm_emit_type(llvm, stmt->idx_var_array.array->type);
      assert(stmt->idx_var_array.output->pseudo);

      LLVMValueRef indexes[] = { LLVMConstInt(LLVMInt32TypeInContext(llvm->context), 0, false), idx };
      stmt->idx_var_array.output->codegen = LLVMBuildGEP2(llvm->builder, array_type, value, indexes, sizeof(indexes) / sizeof(indexes[0]), "");
      return;
    }
    case LOU_HIR_STMT_IDX_PTR: {
      LLVMValueRef idx = lou_llvm_emit_value(llvm, stmt->idx_ptr.value);
      LLVMValueRef value = lou_llvm_emit_value(llvm, stmt->idx_ptr.ptr);
      lou_hir_type_t *type = lou_hir_value_typeof(stmt->idx_ptr.ptr);
      LLVMTypeRef ptr_type = lou_llvm_emit_type(llvm, type->pointer_to);
      assert(stmt->idx_ptr.output->pseudo);
      stmt->idx_ptr.output->codegen = LLVMBuildGEP2(llvm->builder, ptr_type, value, &idx, 1, "");
      return;
    }
    case LOU_HIR_STMT_STRUCT_FIELD:
      lou_llvm_store(llvm, stmt->struct_field.output, LLVMBuildExtractValue(
        llvm->builder,
        lou_llvm_emit_value(llvm, stmt->struct_field.structure),
        stmt->struct_field.idx,
        ""
      ));
      return;
    case LOU_HIR_STMT_VAR_STRUCT_FIELD: {
      assert(stmt->var_struct_field.structure->mutability == LOU_HIR_MUTABLE);
      LLVMValueRef value = stmt->var_struct_field.structure->codegen;
      LLVMTypeRef struct_type = lou_llvm_emit_type(llvm, stmt->var_struct_field.structure->type);
      assert(stmt->var_struct_field.output->pseudo);

      LLVMValueRef indexes[] = {
        LLVMConstInt(LLVMInt32TypeInContext(llvm->context), 0, false),
        LLVMConstInt(LLVMInt32TypeInContext(llvm->context), stmt->var_struct_field.idx, false)
      };
      stmt->var_struct_field.output->codegen = LLVMBuildGEP2(llvm->builder, struct_type, value, indexes, sizeof(indexes) / sizeof(indexes[0]), "");
      return;
    }
  }
  UNREACHABLE();
}

inline void lou_llvm_emit_code_in(lou_llvm_module_t *llvm, LLVMBasicBlockRef block, lou_hir_code_t *code) {
  LLVMPositionBuilderAtEnd(llvm->builder, llvm->loop_vars ? llvm->loop_vars : block);
  for (size_t i = 0; i < lou_vec_len(code->locals); i++) {
    lou_hir_local_t *local = code->locals[i];
    switch (local->mutability) {
      case LOU_HIR_MUTABLE:
        if (!local->pseudo) local->codegen = LLVMBuildAlloca(llvm->builder, lou_llvm_emit_type(llvm, local->type), "");
        break;
      case LOU_HIR_IMMUTABLE: break;
    }
  }

  LLVMPositionBuilderAtEnd(llvm->builder, block);
  for (size_t i = 0; i < lou_vec_len(code->stmts); i++) {
    lou_llvm_emit_stmt(llvm, code->stmts[i]);
  }
}

LLVMBasicBlockRef lou_llvm_emit_code(lou_llvm_module_t *llvm, lou_hir_code_t *code) {
  LLVMBasicBlockRef block = LLVMAppendBasicBlockInContext(llvm->context, llvm->function, "");
  lou_llvm_emit_code_in(llvm, block, code);
  return block;
}

