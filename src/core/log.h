#pragma once

#include <stdarg.h>
#include <stdio.h>

typedef enum {
  LOG_ERROR,
  LOG_WARNING,
  LOG_INFO,
  LOG_DEBUG,
} log_level_t;

typedef void (*log_callback_t)(FILE *stream, va_list list);

void log_register(char id, log_callback_t callback);

void log_fmt_va(log_level_t level, const char *fmt, va_list list);
void log_fmt(log_level_t level, const char *fmt, ...);
