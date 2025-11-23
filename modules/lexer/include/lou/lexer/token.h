#pragma once

#include "lou/core/slice.h"
#include <stdio.h>

typedef enum {
  // Punctuation
  LOU_TOKEN_OPENING_CIRCLE_BRACE, LOU_TOKEN_CLOSING_CIRCLE_BRACE,
  LOU_TOKEN_OPENING_FIGURE_BRACE, LOU_TOKEN_CLOSING_FIGURE_BRACE,
  LOU_TOKEN_OPENING_SQUARE_BRACE, LOU_TOKEN_CLOSING_SQUARE_BRACE,
  LOU_TOKEN_COLON,
  LOU_TOKEN_COMMA,
  LOU_TOKEN_FUNCTION_RETURNS,
  LOU_TOKEN_ASSIGN,

  // Math
  LOU_TOKEN_MINUS,

  // Keywords
  LOU_TOKEN_FUN,
  LOU_TOKEN_PUBLIC,
  LOU_TOKEN_GLOBAL,
  LOU_TOKEN_META, LOU_TOKEN_CONST, LOU_TOKEN_FINAL, LOU_TOKEN_VAR,

  // Other
  LOU_TOKEN_IDENT, LOU_TOKEN_EOI,

  // Internal
  LOU_TOKEN_FAILED,
} lou_token_kind_t;

typedef struct {
  lou_token_kind_t kind;
  lou_slice_t slice;
} lou_token_t;

static inline lou_token_t lou_token_new_simple(lou_slice_t slice, lou_token_kind_t kind) {
  return (lou_token_t) {
    .slice = slice,
    .kind = kind,
  };
}

void lou_token_write(const lou_token_t *token, FILE *stream);
