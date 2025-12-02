#include "value.h"
#include "lou/core/assertions.h"
#include "lou/sema/value.h"
#include <stdio.h>

void lou_sema_value_print(FILE *stream, lou_sema_value_t *value) {
  switch (value->kind) {
    case LOU_SEMA_VALUE_PLUGIN:
      fprintf(stream, "plugin");
      return;
    case LOU_SEMA_VALUE_CONSTANT:
      fprintf(stream, "constant"); // TODO: print type
      return;
    case LOU_SEMA_VALUE_GLOBAL_DECL:
      fprintf(stream, "global declaration"); // TODO: print type
      return;
  }
  UNREACHABLE();
}

