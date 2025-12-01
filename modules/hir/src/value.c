#include "lou/hir/value.h"
#include "lou/core/assertions.h"
#include "lou/hir/decl.h"
#include "lou/hir/func.h"

lou_hir_type_t *lou_hir_value_typeof(lou_hir_value_t *value) {
  switch (value->kind) {
    case LOU_HIR_VALUE_CONST: return value->constant->type;
    case LOU_HIR_VALUE_LOCAL: return value->local->type;
    case LOU_HIR_VALUE_DECL: return value->decl->type;
  }
  UNREACHABLE();
}
