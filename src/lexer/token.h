#pragma once

#include "core/slice.h"

typedef enum {
  // Punctuation
  TOKEN_OPENING_CIRCLE_BRACE, TOKEN_CLOSING_CIRCLE_BRACE,
  TOKEN_OPENING_FIGURE_BRACE, TOKEN_CLOSING_FIGURE_BRACE,
  TOKEN_COLON,
  TOKEN_FUNCTION_RETURNS,

  // Math
  TOKEN_MINUS,

  // Keywords
  TOKEN_FUN,

  // Other
  TOKEN_IDENT,
} token_kind_t;

typedef struct {
  token_kind_t kind;
  slice_t slice;
} token_t;

static inline token_t token_create_simple(slice_t slice, token_kind_t kind) {
  return (token_t) {
    .slice = slice,
    .kind = kind,
  };
}
