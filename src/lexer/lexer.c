#include "lexer.h"
#include "chars.h"
#include "core/io.h"
#include "core/log.h"
#include "core/mempool.h"
#include "core/slice.h"
#include "lexer/chars.h"
#include <stdio.h>

#define EOI ((char)-1)

typedef struct lexer_t {
  mempool_t *mempool; 
  const char *path;
  slice_t content;
  size_t start_pos, pos;
  bool failed;
} lexer_t;

lexer_t *lexer_create(const char *path) {
  mempool_t *mempool = mempool_new();
  lexer_t *lexer = MEMPOOL_ALLOC(mempool, lexer_t);
  lexer->mempool = mempool;
  lexer->failed = false;
  lexer->path = path;
  lexer->start_pos = lexer->pos = 0;
  
  FILE *file = fopen(path, "r");
  if (!file) {
    log_fmt(LOG_ERROR, "failed to open the file: #E"); 
    goto error;
  }

  if (!file_read_all(mempool, file, &lexer->content)) {
    log_fmt(LOG_ERROR, "failed to read file content: #E"); 
    goto error;
  }
  fclose(file);

  return lexer;

  error: {
    mempool_free(mempool);
    return NULL;
  }
}

void lexer_free(lexer_t *lexer) {
  mempool_free(lexer->mempool);
}

static inline char lexer_peek(const lexer_t *lexer) {
  if (lexer->pos >= lexer->content.length) {
    return EOI;
  }
  return lexer->content.ptr[lexer->pos];
}

static inline char lexer_take(lexer_t *lexer) {
  if (lexer->pos >= lexer->content.length) {
    return EOI;
  }
  return lexer->content.ptr[lexer->pos++];
}

static inline bool lexer_take_if(lexer_t *lexer, char c) {
  if (lexer_peek(lexer) == c) {
    lexer_take(lexer);
    return true;
  }
  return false;
}

static inline slice_t lexer_slice(const lexer_t *lexer) {
  return subslice(lexer->content, lexer->start_pos, lexer->pos);
}

static inline void lexer_begin(lexer_t *lexer) {
  lexer->start_pos = lexer->pos;
}

static inline void lexer_skip(lexer_t *lexer, bool (*filter)(char c)) {
  for (char c = lexer_peek(lexer); filter(c) && c != EOI; c = lexer_peek(lexer)) {
    lexer_take(lexer);
  }
}

static inline void lexer_skip_non_token(lexer_t *lexer) {
  size_t pos;
  do {
    pos = lexer->pos;
    lexer_skip(lexer, char_is_whitespace);
    if (lexer_take_if(lexer, '#')) {
      lexer_skip(lexer, char_is_not_nl);
    }
  } while (pos != lexer->pos);
}

bool lexer_failed(const lexer_t *lexer) {
  return lexer->failed;
}

static inline void lexer_error(lexer_t *lexer, const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);
  log_fmt_va(LOG_ERROR, fmt, list);
  slice_t slice = lexer_slice(lexer);
  fputs("at `", stderr);
  fwrite(slice.ptr, 1, slice.length, stderr);
  fputs("`\n", stderr);
  va_end(list);

  lexer->failed = true;
}

token_t lexer_next(lexer_t *lexer) {
  bool locally_failed = false;
  while (true) {
    lexer_skip_non_token(lexer);
    lexer_begin(lexer);
    char c = lexer_take(lexer);
    switch (c) {
      case '(': return token_new_simple(lexer_slice(lexer), TOKEN_OPENING_CIRCLE_BRACE);
      case ')': return token_new_simple(lexer_slice(lexer), TOKEN_CLOSING_CIRCLE_BRACE);
      case '[': return token_new_simple(lexer_slice(lexer), TOKEN_OPENING_SQUARE_BRACE);
      case ']': return token_new_simple(lexer_slice(lexer), TOKEN_CLOSING_SQUARE_BRACE);
      case '{': return token_new_simple(lexer_slice(lexer), TOKEN_OPENING_FIGURE_BRACE);
      case '}': return token_new_simple(lexer_slice(lexer), TOKEN_CLOSING_FIGURE_BRACE);
      case ':': return token_new_simple(lexer_slice(lexer), TOKEN_COLON);
      case '-':
        if (lexer_take_if(lexer, '>')) {
          return token_new_simple(lexer_slice(lexer), TOKEN_FUNCTION_RETURNS);
        }
        return token_new_simple(lexer_slice(lexer), TOKEN_MINUS);
      case EOI:
        return token_new_simple(lexer_slice(lexer), TOKEN_EOI);

      default: {
        if (char_is_ident_start(c)) {
          lexer_skip(lexer, char_is_ident);
          return token_new_simple(lexer_slice(lexer), TOKEN_IDENT);
        }
        lexer_error(lexer, "unknown symbol");
        break;
      }
    }
  }
}
