#pragma once

#include "lou/core/slice.h"
#include <stdint.h>
#include <stdio.h>

typedef enum {
  // Punctuation
  LOU_TOKEN_OPENING_CIRCLE_BRACE, LOU_TOKEN_CLOSING_CIRCLE_BRACE,
  LOU_TOKEN_OPENING_FIGURE_BRACE, LOU_TOKEN_CLOSING_FIGURE_BRACE,
  LOU_TOKEN_OPENING_SQUARE_BRACE, LOU_TOKEN_CLOSING_SQUARE_BRACE,
  LOU_TOKEN_SEMICOLON,
  LOU_TOKEN_COLON,
  LOU_TOKEN_COMMA,
  LOU_TOKEN_FUNCTION_RETURNS,
  LOU_TOKEN_ASSIGN,
  LOU_TOKEN_DOT,
  LOU_TOKEN_EXCLAMATION,

  // Math
  LOU_TOKEN_PLUS,
  LOU_TOKEN_MINUS,
  LOU_TOKEN_STAR,
  LOU_TOKEN_SLASH,
  LOU_TOKEN_PERCENT,
  LOU_TOKEN_EQUALS, LOU_TOKEN_NOT_EQUALS,
  LOU_TOKEN_GREATER, LOU_TOKEN_LESS,
  LOU_TOKEN_GREATER_OR_EQUALS, LOU_TOKEN_LESS_OR_EQUALS,

  LOU_TOKEN_PLUS_ASSIGN,
  LOU_TOKEN_MINUS_ASSIGN,
  LOU_TOKEN_STAR_ASSIGN,
  LOU_TOKEN_SLASH_ASSIGN,
  LOU_TOKEN_PERCENT_ASSIGN,

  // Keywords
  LOU_TOKEN_FUN,
  LOU_TOKEN_PUBLIC,
  LOU_TOKEN_META, LOU_TOKEN_CONST, LOU_TOKEN_FINAL, LOU_TOKEN_VAR,
  LOU_TOKEN_RETURN,
  LOU_TOKEN_IF, LOU_TOKEN_ELSE,
  LOU_TOKEN_WHILE,
  LOU_TOKEN_STRUCT,

  // Other
  LOU_TOKEN_IDENT,

  // Complex
  LOU_TOKEN_INTEGER,
  LOU_TOKEN_CHAR,
  LOU_TOKEN_STRING,

  // Internal
  LOU_TOKEN_EOI, LOU_TOKEN_FAILED,
} lou_token_kind_t;

typedef enum {
  LOU_TOKEN_STRING_NORMAL,
  LOU_TOKEN_STRING_C,
} lou_token_string_kind_t;

typedef struct {
  lou_token_kind_t kind;
  size_t line;
  lou_slice_t slice;

  union {
    uint64_t integer;
    char character;
    struct {
      lou_slice_t content;
      lou_token_string_kind_t kind;
    } string;
  };
} lou_token_t;

static inline lou_token_t lou_token_new_simple(lou_slice_t slice, size_t line, lou_token_kind_t kind) {
  return (lou_token_t) {
    .slice = slice,
    .line = line,
    .kind = kind,
  };
}

static inline lou_token_t lou_token_new_char(lou_slice_t slice, size_t line, char character) {
  return (lou_token_t) {
    .slice = slice,
    .line = line,
    .kind = LOU_TOKEN_CHAR,
    .character = character,
  };
}

static inline lou_token_t lou_token_new_string(lou_slice_t slice, size_t line, lou_slice_t string, lou_token_string_kind_t kind) {
  return (lou_token_t) {
    .slice = slice,
    .line = line,
    .kind = LOU_TOKEN_STRING,
    .string = {
      .kind = kind,
      .content = string,
    }
  };
}

static inline lou_token_t lou_token_new_integer(lou_slice_t slice, size_t line, uint64_t integer) {
  return (lou_token_t) {
    .slice = slice,
    .line = line,
    .kind = LOU_TOKEN_INTEGER,
    .integer = integer,
  };
}

void lou_token_write(const lou_token_t *token, FILE *stream);
