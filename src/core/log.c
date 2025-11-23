#include "log.h"
#include <stdio.h>

static log_callback_t log_callbacks[0xFF] = { NULL };

typedef struct {
  FILE *output;
  const char *name;
  int color;
} log_level_info_t;

void log_register(char id, log_callback_t callback) {
  log_callbacks[(int)id] = callback;
}

inline void log_fmt_va(log_level_t level, const char *fmt, va_list list) {
  log_level_info_t infos[] = {
    [LOG_DEBUG]   = { .output = stdout, .name = "debug",   .color = 34 },
    [LOG_INFO]    = { .output = stdout, .name = "info",    .color = 32 },
    [LOG_WARNING] = { .output = stderr, .name = "warning", .color = 33 },
    [LOG_ERROR]   = { .output = stderr, .name = "error",   .color = 31 },
  };

  log_level_info_t *info = &infos[level];

  fprintf(info->output, "\033[1;%dm" "%s: " "\033[0m", info->color, info->name);

  for (size_t i = 0; fmt[i]; i++) {
    log_callback_t callback = log_callbacks[(int)fmt[i + 1]];
    if (fmt[i] == '#' && callback) {
      callback(info->output, list);
      i++;
    } else {
      fputc(fmt[i], info->output);
    }
  }
  fputc('\n', info->output);
}

void log_fmt(log_level_t level, const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);
  log_fmt_va(level, fmt, list);
  va_end(list);
}

