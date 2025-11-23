#pragma once

#include <stdbool.h>

static inline bool char_is_whitespace(char c) {
  return c == ' ' || c == '\t' || c == '\n';
}

static inline bool char_is_digit(char c) {
  return c >= '0' && c <= '9';
}

static inline bool char_is_ident_start(char c) {
  return
    c >= 'a' && c <= 'z' || 
    c >= 'A' && c <= 'Z' ||
    c == '_' ||
    c == '$';
}

static inline bool char_is_not_nl(char c) {
  return c != '\n';
}

static inline bool char_is_ident(char c) {
  return char_is_ident_start(c) || char_is_digit(c);
}
