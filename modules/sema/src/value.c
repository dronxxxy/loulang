#include "value.h"
#include "const.h"
#include "lou/core/assertions.h"
#include "lou/sema/value.h"
#include "sema.h"
#include <stdio.h>

void lou_sema_value_print(FILE *stream, lou_sema_value_t *value) {
  switch (value->kind) {
    case LOU_SEMA_VALUE_PLUGIN:
      fprintf(stream, "plugin");
      return;
    case LOU_SEMA_VALUE_CONSTANT:
      fprintf(stream, "constant"); // TODO: print type
      return;
    case LOU_SEMA_VALUE_DECL:
      fprintf(stream, "global declaration"); // TODO: print type
      return;
    case LOU_SEMA_VALUE_TYPE:
      fprintf(stream, "type");
      return;
  }
  UNREACHABLE();
}

lou_slice_t *lou_sema_value_is_const_string(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_CONSTANT) return NULL;
  if (value->constant->kind != LOU_SEMA_CONST_STRING) return NULL;
  return &value->constant->string;
}

lou_sema_const_func_t *lou_sema_value_is_const_func(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_CONSTANT) return NULL;
  if (value->constant->kind != LOU_SEMA_CONST_FUNCTION) return NULL;
  return &value->constant->func;
}

lou_sema_type_t *lou_sema_value_is_type(lou_sema_value_t *value) {
  if (value->kind != LOU_SEMA_VALUE_TYPE) return NULL;
  return value->type;
}

lou_sema_type_t *lou_sema_value_is_runtime(lou_sema_value_t *value) {
  switch (value->kind) {
    case LOU_SEMA_VALUE_PLUGIN:
      return NULL;
    case LOU_SEMA_VALUE_CONSTANT:
      return value->constant->type;
    case LOU_SEMA_VALUE_DECL:
      return value->decl->type;
    case LOU_SEMA_VALUE_TYPE:
      return NULL;
  }
  UNREACHABLE();
}
