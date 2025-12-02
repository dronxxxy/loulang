#include "log.h"
#include "lou/core/log.h"
#include "lou/lexer/token.h"
#include "lou/sema/value.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

static void log_sharp(FILE *stream, va_list list) {
  fputc('#', stream);
}

static void log_errno(FILE *stream, va_list list) {
  fputs(strerror(errno), stream);
}

static void log_slice(FILE *stream, va_list list) {
  lou_slice_t slice = va_arg(list, lou_slice_t);
  fwrite(slice.ptr, 1, slice.length, stream);
}

static void log_token(FILE *stream, va_list list) {
  lou_token_write(va_arg(list, lou_token_t *), stream);
}

static void log_cstr(FILE *stream, va_list list) {
  fprintf(stream, "%s", va_arg(list, const char *));
}

static void log_size_t(FILE *stream, va_list list) {
  fprintf(stream, "%lu", va_arg(list, size_t));
}

static void log_sema_value(FILE *stream, va_list list) {
  lou_sema_value_print(stream, va_arg(list, lou_sema_value_t *));
}

void log_init() {
  lou_log_register('#', log_sharp);
  lou_log_register('E', log_errno);
  lou_log_register('T', log_token);
  lou_log_register('S', log_slice);
  lou_log_register('s', log_cstr);
  lou_log_register('l', log_size_t);
  lou_log_register('v', log_sema_value);
}
