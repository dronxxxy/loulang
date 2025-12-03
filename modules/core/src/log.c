#include "lou/core/log.h"
#include <stdio.h>

static lou_log_callback_t log_callbacks[0xFF] = { NULL };

typedef struct {
  FILE *output;
  const char *name;
  int color;
} log_level_info_t;

void lou_log_register(char id, lou_log_callback_t callback) {
  log_callbacks[(int)id] = callback;
}

inline void lou_log_puts_va(FILE *stream, const char *fmt, va_list list) {
  for (size_t i = 0; fmt[i]; i++) {
    lou_log_callback_t callback = log_callbacks[(int)fmt[i + 1]];
    if (fmt[i] == '#' && callback) {
      callback(stream, list);
      i++;
    } else {
      fputc(fmt[i], stream);
    }
  }
}

inline void lou_log_fmt_va(lou_log_level_t level, const char *fmt, va_list list) {
  log_level_info_t infos[] = {
    [LOG_DEBUG]   = { .output = stdout, .name = "debug",   .color = 34 },
    [LOG_INFO]    = { .output = stdout, .name = "info",    .color = 32 },
    [LOG_WARNING] = { .output = stderr, .name = "warning", .color = 33 },
    [LOG_ERROR]   = { .output = stderr, .name = "error",   .color = 31 },
  };

  log_level_info_t *info = &infos[level];

  fprintf(info->output, "\033[1;%dm" "%s: " "\033[0m", info->color, info->name);
  lou_log_puts_va(info->output, fmt, list);
  fputc('\n', info->output);
}

void lou_log_fmt(lou_log_level_t level, const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);
  lou_log_fmt_va(level, fmt, list);
  va_end(list);
}

void lou_log_puts(FILE *stream, const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);
  lou_log_puts_va(stream, fmt, list);
  va_end(list);
}
