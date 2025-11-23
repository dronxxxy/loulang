#include "lou/lexer/lexer.h"
#include "chars.h"
#include "lou/core/io.h"
#include "lou/core/log.h"
#include "lou/core/mempool.h"
#include "lou/core/pos_print.h"
#include "lou/core/slice.h"
#include <stdio.h>
#include <string.h>

#define EOI ((char)-1)
#define UNKNOWN_TOKEN_MESSAGE "unknown token"

typedef struct lou_lexer_t {
  lou_mempool_t *mempool; 
  lou_slice_t path;
  lou_slice_t content;
  size_t start_pos, pos;
  bool failed;
} lou_lexer_t;

lou_lexer_t *lou_lexer_new(lou_slice_t path) {
  lou_mempool_t *mempool = lou_mempool_new();
  lou_lexer_t *lexer = LOU_MEMPOOL_ALLOC(mempool, lou_lexer_t);
  lexer->mempool = mempool;
  lexer->failed = false;
  lexer->path = path;
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
  return lexer->content.ptr[lexer->pos++];
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
}

static inline void lexer_skip(lou_lexer_t *lexer, bool (*filter)(char c)) {
  for (char c = lou_lexer_peek(lexer); filter(c) && c != EOI; c = lou_lexer_peek(lexer)) {
    lou_lexer_take(lexer);
  }
}

static inline void lou_lexer_skip_non_token(lou_lexer_t *lexer) {
  size_t pos;
  do {
    pos = lexer->pos;
    lexer_skip(lexer, char_is_whitespace);
    if (lou_lexer_take_if(lexer, '#')) {
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

static inline void lou_lexer_error(lou_lexer_t *lexer, lou_slice_t slice, const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);
  lou_lexer_log_error(lexer, slice, fmt, list);
  va_end(list);

  lexer->failed = true;
}

static inline lou_token_t lou_lexer_try_next(lou_lexer_t *lexer) {
  lou_lexer_begin(lexer);
  char c = lou_lexer_take(lexer);
  switch (c) {
    case '(': return lou_token_new_simple(lou_lexer_slice(lexer), LOU_TOKEN_OPENING_CIRCLE_BRACE);
    case ')': return lou_token_new_simple(lou_lexer_slice(lexer), LOU_TOKEN_CLOSING_CIRCLE_BRACE);
    case '[': return lou_token_new_simple(lou_lexer_slice(lexer), LOU_TOKEN_OPENING_SQUARE_BRACE);
    case ']': return lou_token_new_simple(lou_lexer_slice(lexer), LOU_TOKEN_CLOSING_SQUARE_BRACE);
    case '{': return lou_token_new_simple(lou_lexer_slice(lexer), LOU_TOKEN_OPENING_FIGURE_BRACE);
    case '}': return lou_token_new_simple(lou_lexer_slice(lexer), LOU_TOKEN_CLOSING_FIGURE_BRACE);
    case ':': return lou_token_new_simple(lou_lexer_slice(lexer), LOU_TOKEN_COLON);
    case EOI: return lou_token_new_simple(lou_lexer_slice(lexer), LOU_TOKEN_EOI);
    case '-':
      if (lou_lexer_take_if(lexer, '>')) {
        return lou_token_new_simple(lou_lexer_slice(lexer), LOU_TOKEN_FUNCTION_RETURNS);
      }
      return lou_token_new_simple(lou_lexer_slice(lexer), LOU_TOKEN_MINUS);

    default: {
      if (char_is_ident_start(c)) {
        lexer_skip(lexer, char_is_ident);
        return lou_token_new_simple(lou_lexer_slice(lexer), LOU_TOKEN_IDENT);
      }
      return lou_token_new_simple(lou_lexer_slice(lexer), LOU_TOKEN_FAILED);
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
