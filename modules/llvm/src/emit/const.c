#include "const.h"
#include "emit/type.h"
#include "lou/core/assertions.h"
#include "lou/hir/const.h"
#include "lou/hir/extern.h"
#include "lou/hir/func.h"
#include "lou/hir/type.h"
#include "lou/llvm/module.h"
#include "utils.h"
#include <assert.h>
#include <llvm-c/Core.h>

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
      return lou_llvm_function_decl(llvm, constant->func->name, constant->type);
    case LOU_HIR_CONST_EXTERNAL:
      return constant->external->codegen;
  }
  UNREACHABLE();
}
