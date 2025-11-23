#include "log.h"
#include "core/log.h"
#include "lexer/token.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

static void log_sharp(FILE *stream, va_list list) {
  fputc('#', stream);
}

static void log_errno(FILE *stream, va_list list) {
  fputs(strerror(errno), stream);
}

static void log_token(FILE *stream, va_list list) {
  token_write(va_arg(list, token_t *), stream);
}

void log_init() {
  log_register('#', log_sharp);
  log_register('E', log_errno);
  log_register('T', log_token);
}
