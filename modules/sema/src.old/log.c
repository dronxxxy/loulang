#include "lou/sema/log.h"
#include "lou/core/log.h"
#include "value.h"

static void log_sema_value(FILE *stream, va_list list) {
  lou_sema_value_print(stream, va_arg(list, lou_sema_value_t*));
}

static void log_sema_type(FILE *stream, va_list list) {
  lou_sema_type_print(stream, va_arg(list, lou_sema_type_t*));
}

void lou_sema_init_log() {
  lou_log_register('v', log_sema_value);
  lou_log_register('T', log_sema_type);
}
