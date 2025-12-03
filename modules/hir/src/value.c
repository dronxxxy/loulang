#include "lou/hir/value.h"
#include "lou/core/assertions.h"
#include "lou/hir/code.h"
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

lou_hir_mutability_t lou_hir_value_mutability(lou_hir_value_t *value) {
  switch (value->kind) {
    case LOU_HIR_VALUE_CONST: return LOU_HIR_IMMUTABLE;
    case LOU_HIR_VALUE_LOCAL: return value->local->mutability;
    case LOU_HIR_VALUE_DECL: return value->decl->mutability;
  }
  UNREACHABLE();
}
