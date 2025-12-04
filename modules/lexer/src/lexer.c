#include "lou/lexer/lexer.h"
#include "chars.h"
#include "lou/core/io.h"
#include "lou/core/log.h"
#include "lou/core/mempool.h"
#include "lou/core/pos_print.h"
#include "lou/core/slice.h"
#include "lou/lexer/token.h"
#include <stdio.h>
#include <string.h>

#define EOI ((char)-1)
#define UNKNOWN_TOKEN_MESSAGE "unknown token"

typedef struct lou_lexer_t {
  lou_mempool_t *mempool; 
  lou_slice_t path;
  lou_slice_t content;
  size_t start_line, line;
  size_t start_pos, pos;
  bool failed;
} lou_lexer_t;

lou_lexer_t *lou_lexer_new(lou_slice_t path) {
  lou_mempool_t *mempool = lou_mempool_new();
  lou_lexer_t *lexer = LOU_MEMPOOL_ALLOC(mempool, lou_lexer_t);
  lexer->mempool = mempool;
  lexer->failed = false;
  lexer->path = path;
  lexer->start_line = lexer->line = 0;
  lexer->start_pos = lexer->pos = 0;
  
  char *new_path = lou_mempool_alloc(mempool, path.length + 1, 1);
  memcpy(new_path, path.ptr, path.length);
  new_path[path.length] = '\0';
  FILE *file = fopen(new_path, "r");
  if (!file) {
    lou_log_fmt(LOG_ERROR, "failed to open the file: #E"); 
    goto error;
  }

  if (!lou_file_read_all(mempool, file, &lexer->content)) {
    lou_log_fmt(LOG_ERROR, "failed to read file content: #E"); 
    goto error;
  }
  fclose(file);

  return lexer;

  error: {
    lou_mempool_free(mempool);
    return NULL;
  }
}

void lou_lexer_free(lou_lexer_t *lexer) {
  lou_mempool_free(lexer->mempool);
}

static inline char lou_lexer_peek(const lou_lexer_t *lexer) {
  if (lexer->pos >= lexer->content.length) {
    return EOI;
  }
  return lexer->content.ptr[lexer->pos];
}

static inline char lou_lexer_take(lou_lexer_t *lexer) {
  if (lexer->pos >= lexer->content.length) {
    return EOI;
  }
  char c = lexer->content.ptr[lexer->pos++];
  if (c == '\n') {
    lexer->line++;
  }
  return c;
}

static inline bool lou_lexer_take_if(lou_lexer_t *lexer, char c) {
  if (lou_lexer_peek(lexer) == c) {
    lou_lexer_take(lexer);
    return true;
  }
  return false;
}

static inline lou_slice_t lou_lexer_slice(const lou_lexer_t *lexer) {
  return lou_subslice(lexer->content, lexer->start_pos, lexer->pos);
}

static inline void lou_lexer_begin(lou_lexer_t *lexer) {
  lexer->start_pos = lexer->pos;
  lexer->start_line = lexer->line;
}

static inline void lexer_skip(lou_lexer_t *lexer, bool (*filter)(char c)) {
  for (char c = lou_lexer_peek(lexer); filter(c) && c != EOI; c = lou_lexer_peek(lexer)) {
    lou_lexer_take(lexer);
  }
}

static inline void lou_lexer_error(lou_lexer_t *lexer, lou_slice_t slice, const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);
  lou_lexer_log_error(lexer, slice, fmt, list);
  va_end(list);

  lexer->failed = true;
}

static inline void lou_lexer_skip_non_token(lou_lexer_t *lexer) {
  size_t pos;
  do {
    pos = lexer->pos;
    lexer_skip(lexer, char_is_whitespace);
    lou_lexer_begin(lexer);
    if (lou_lexer_take_if(lexer, '#')) {
      if (lou_lexer_take_if(lexer, '`')) {
        while (!(lou_lexer_take_if(lexer, '`') && lou_lexer_take_if(lexer, '#'))) {
          if (lou_lexer_take(lexer) == EOI) {
            lou_lexer_error(lexer, lou_lexer_slice(lexer), "expected end of comment");
            break;
          }
        }
      }
      lexer_skip(lexer, char_is_not_nl);
    }
  } while (pos != lexer->pos);
}

bool lou_lexer_failed(const lou_lexer_t *lexer) {
  return lexer->failed;
}

void lou_lexer_log_error(const lou_lexer_t *lexer, lou_slice_t slice, const char *fmt, va_list list) {
  lou_log_fmt_va(LOG_ERROR, fmt, list);
  lou_pos_print(stderr, lexer->path, lexer->content, slice);
  fprintf(stderr, "\n\n");
}

static inline lou_token_t lou_lexer_new_simple(lou_lexer_t *lexer, lou_token_kind_t kind) {
  return lou_token_new_simple(lou_lexer_slice(lexer), lexer->start_line, kind);
}

typedef struct {
  const char *name;
  lou_token_kind_t kind;
} lou_lexer_keyword_t;

static lou_lexer_keyword_t lou_lexer_keywords[] = {
  { .name = "public", .kind = LOU_TOKEN_PUBLIC },
  { .name = "fun", .kind = LOU_TOKEN_FUN },
  { .name = "meta", .kind = LOU_TOKEN_META },
  { .name = "const", .kind = LOU_TOKEN_CONST },
  { .name = "final", .kind = LOU_TOKEN_FINAL },
  { .name = "var", .kind = LOU_TOKEN_VAR },
  { .name = "return", .kind = LOU_TOKEN_RETURN },
  { .name = "if", .kind = LOU_TOKEN_IF },
};

static inline lou_token_t lou_lexer_try_next(lou_lexer_t *lexer) {
  lou_lexer_begin(lexer);
  char c = lou_lexer_take(lexer);
  switch (c) {
    case '(': return lou_lexer_new_simple(lexer, LOU_TOKEN_OPENING_CIRCLE_BRACE);
    case ')': return lou_lexer_new_simple(lexer, LOU_TOKEN_CLOSING_CIRCLE_BRACE);
    case '[': return lou_lexer_new_simple(lexer, LOU_TOKEN_OPENING_SQUARE_BRACE);
    case ']': return lou_lexer_new_simple(lexer, LOU_TOKEN_CLOSING_SQUARE_BRACE);
    case '{': return lou_lexer_new_simple(lexer, LOU_TOKEN_OPENING_FIGURE_BRACE);
    case '}': return lou_lexer_new_simple(lexer, LOU_TOKEN_CLOSING_FIGURE_BRACE);
    case ':': return lou_lexer_new_simple(lexer, LOU_TOKEN_COLON);
    case '=': return lou_lexer_new_simple(lexer, LOU_TOKEN_ASSIGN);
    case ',': return lou_lexer_new_simple(lexer, LOU_TOKEN_COMMA);
    case '.': return lou_lexer_new_simple(lexer, LOU_TOKEN_DOT);
    case '"': {
      lexer_skip(lexer, char_is_not_string_end);
      lou_lexer_take(lexer);
      // TODO: better strings lexing
      lou_slice_t string = lou_lexer_slice(lexer);
      string.ptr++;
      string.length -= 2;
      return lou_token_new_string(lou_lexer_slice(lexer), lexer->start_line, string);
    }
    case EOI: return lou_lexer_new_simple(lexer, LOU_TOKEN_EOI);
    case '-':
      if (lou_lexer_take_if(lexer, '>')) {
        return lou_lexer_new_simple(lexer, LOU_TOKEN_FUNCTION_RETURNS);
      }
      return lou_lexer_new_simple(lexer, LOU_TOKEN_MINUS);

    default: {
      if (char_is_ident_start(c)) {
        lexer_skip(lexer, char_is_ident);
        lou_slice_t slice = lou_lexer_slice(lexer);
        for (size_t i = 0; i < sizeof(lou_lexer_keywords) / sizeof(lou_lexer_keyword_t); i++) {
          lou_lexer_keyword_t *keyword = &lou_lexer_keywords[i];
          if (lou_slice_eq(lou_slice_from_cstr(keyword->name), slice)) {
            return lou_lexer_new_simple(lexer, keyword->kind);
          }
        }
        return lou_lexer_new_simple(lexer, LOU_TOKEN_IDENT);
      }
      if (char_is_digit(c)) {
        lexer_skip(lexer, char_is_digit);
        lou_slice_t slice = lou_lexer_slice(lexer);
        uint64_t result = 0;
        for (size_t i = 0; i < slice.length; i++) {
          result *= 10;
          result += slice.ptr[i] - '0';
        }
        return lou_token_new_integer(lou_lexer_slice(lexer), lexer->start_line, result);
      }
      return lou_lexer_new_simple(lexer, LOU_TOKEN_FAILED);
    }
  }
}

lou_token_t lou_lexer_next(lou_lexer_t *lexer) {
  lou_slice_t failed_at = lou_slice_new(NULL, 0);
  while (true) {
    lou_lexer_skip_non_token(lexer);
    lou_token_t token = lou_lexer_try_next(lexer);
    if (token.kind != LOU_TOKEN_FAILED) {
      if (failed_at.ptr) {
        lou_lexer_error(lexer, failed_at, UNKNOWN_TOKEN_MESSAGE);
      }
      return token; 
    }
    if (!failed_at.ptr) {
      failed_at = token.slice;
    } else if (token.slice.ptr - failed_at.ptr == failed_at.length) {
      failed_at.length += token.slice.length;
    } else {
      lou_lexer_error(lexer, failed_at, UNKNOWN_TOKEN_MESSAGE);
      failed_at = token.slice;
    }
  }
}
