#include "lou/core/log.h"
#include "type.h"
#include "value.h"
#include <stdarg.h>
#include <stdio.h>

void lou_sema_log_type(FILE *stream, va_list list) {
  lou_sema_type_log(stream, va_arg(list, lou_sema_type_t*));
}

void lou_sema_log_value(FILE *stream, va_list list) {
  lou_sema_value_log(stream, va_arg(list, lou_sema_value_t*));
}

void lou_sema_init_log() {
  lou_log_register('T', lou_sema_log_type);
  lou_log_register('V', lou_sema_log_value);
}
