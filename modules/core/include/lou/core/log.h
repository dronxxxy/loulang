#pragma once

#include <stdarg.h>
#include <stdio.h>

typedef enum {
  LOG_ERROR,
  LOG_WARNING,
  LOG_INFO,
  LOG_DEBUG,
} lou_log_level_t;

typedef void (*lou_log_callback_t)(FILE *stream, va_list list);

void lou_log_register(char id, lou_log_callback_t callback);

void lou_log_puts_va(FILE *stream, const char *fmt, va_list list);
void lou_log_puts(FILE *stream, const char *fmt, ...);

void lou_log_fmt_va(lou_log_level_t level, const char *fmt, va_list list);
void lou_log_fmt(lou_log_level_t level, const char *fmt, ...);
