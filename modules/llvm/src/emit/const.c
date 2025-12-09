#include "const.h"
#include "emit/type.h"
#include "lou/core/assertions.h"
#include "lou/hir/const.h"
#include "lou/hir/extern.h"
#include "lou/hir/func.h"
#include "lou/hir/type.h"
#include "lou/llvm/module.h"
#include <assert.h>
#include <llvm-c/Core.h>

LLVMValueRef lou_llvm_emit_const(lou_llvm_module_t *llvm, const lou_hir_const_t *constant) {
  switch (constant->kind) {
    case LOU_HIR_CONST_INTEGER:
      assert(constant->type->kind == LOU_HIR_TYPE_INT);
      return LLVMConstInt(lou_llvm_emit_type(llvm, constant->type), constant->integer.value, constant->type->integer.is_signed);
    case LOU_HIR_CONST_FUNCTION: return constant->func->codegen;
    case LOU_HIR_CONST_EXTERNAL: return constant->external->codegen;
    case LOU_HIR_CONST_STRING: {
      LLVMValueRef string = LLVMAddGlobal(llvm->module, LLVMPointerTypeInContext(llvm->context, 0), "");
      LLVMSetInitializer(string, LLVMConstStringInContext(llvm->context,
        constant->string.content.ptr,
        constant->string.content.length,
        constant->string.null_terminated
      ));
      return string;
    }
  }
  UNREACHABLE();
}
