#include "const.h"
#include "emit/code.h"
#include "emit/type.h"
#include "lou/core/assertions.h"
#include "lou/hir/const.h"
#include "lou/hir/func.h"
#include "lou/hir/type.h"
#include "lou/llvm/module.h"
#include "utils.h"
#include <assert.h>
#include <llvm-c/Core.h>

static inline LLVMValueRef lou_llvm_emit_function(lou_llvm_module_t *llvm, lou_hir_func_t *func, lou_hir_type_t *type) {
  assert(type->kind == LOU_HIR_TYPE_FUNC);
  LLVMValueRef function = lou_llvm_function_decl(llvm, func->name, type);
  assert(!llvm->function);
  llvm->function = function;
  lou_llvm_emit_code(llvm, func->code);
  llvm->function = NULL;
  return function;
}

LLVMValueRef lou_llvm_emit_const(lou_llvm_module_t *llvm, const lou_hir_const_t *constant) {
  switch (constant->kind) {
    case LOU_HIR_CONST_INTEGER:
      assert(constant->type->kind == LOU_HIR_TYPE_INT);
      return LLVMConstInt(
        lou_llvm_emit_type(llvm, constant->type),
        constant->integer.value,
        constant->type->integer.is_signed
      );
    case LOU_HIR_CONST_FUNCTION:
      return lou_llvm_emit_function(llvm, constant->func, constant->type);
    case LOU_HIR_CONST_EXTERNAL:
      return lou_llvm_function_decl(llvm, (lou_opt_slice_t){ true, constant->external.name }, constant->type);
    }
  UNREACHABLE();
}
