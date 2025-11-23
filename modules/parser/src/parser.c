#include "parser.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/lexer/lexer.h"
#include "lou/lexer/token.h"
#include "lou/parser/parser.h"
#include <alloca.h>
#include <stdarg.h>

void lou_parser_err(lou_parser_t *parser, lou_slice_t slice, const char *fmt, ...) {
  va_list list;
  va_start(list, fmt);
  lou_lexer_log_error(parser->lexer, slice, fmt, list);
  va_end(list);

  parser->failed = true;
}

static inline lou_token_t lou_parser_queue(lou_parser_t *parser, lou_token_t token) {
  *LOU_VEC_PUSH(&parser->queue) = token;
  return token;
}

lou_token_t lou_parser_take(lou_parser_t *parser) {
  if (lou_vec_len(parser->queue) != 0) {
    lou_token_t token = parser->queue[0];
    LOU_VEC_UNSHIFT(&parser->queue);
    return token;
  }
  return lou_lexer_next(parser->lexer);
}

lou_token_t lou_parser_peek(lou_parser_t *parser) {
  if (lou_vec_len(parser->queue) != 0) {
    return parser->queue[0];
  }
  return lou_parser_queue(parser, lou_lexer_next(parser->lexer));
}

bool lou_parser_expect(lou_parser_t *parser, lou_token_kind_t kind, lou_token_t *output) {
  lou_token_t token = lou_parser_peek(parser);
  if (token.kind == kind) {
    *output = lou_parser_take(parser);
    return true;
  }
  lou_token_t reference = lou_token_new_simple(lou_slice_from_cstr("_"), 1, kind);
  lou_parser_err(parser, token.slice, "expected #T", &reference);
  return false;
}

bool lou_parser_take_if(lou_parser_t *parser, lou_token_kind_t kind) {
  if (lou_parser_peek(parser).kind == kind) {
    lou_parser_take(parser);
    return true;
  }
  return false;
}

bool lou_parser_finished(lou_parser_t *parser) {
  return lou_parser_peek(parser).kind == LOU_TOKEN_EOI;
}

static inline lou_token_kind_t lou_parser_closing_to_opening(lou_token_kind_t kind) {
  switch (kind) {
    case LOU_TOKEN_CLOSING_CIRCLE_BRACE: return LOU_TOKEN_OPENING_CIRCLE_BRACE;
    case LOU_TOKEN_CLOSING_FIGURE_BRACE: return LOU_TOKEN_OPENING_FIGURE_BRACE;
    case LOU_TOKEN_CLOSING_SQUARE_BRACE: return LOU_TOKEN_OPENING_SQUARE_BRACE;
    default: return kind;
  }
}

static inline void lou_parser_brace_factor(lou_token_kind_t **braces, lou_token_kind_t kind) {
  switch (kind) {
    case LOU_TOKEN_OPENING_CIRCLE_BRACE: case LOU_TOKEN_OPENING_FIGURE_BRACE: case LOU_TOKEN_OPENING_SQUARE_BRACE:
      *LOU_VEC_PUSH(braces) = kind;
      break;
    case LOU_TOKEN_CLOSING_CIRCLE_BRACE: case LOU_TOKEN_CLOSING_FIGURE_BRACE: case LOU_TOKEN_CLOSING_SQUARE_BRACE:
      if (lou_vec_len(*braces) && *LOU_VEC_LAST(*braces) == lou_parser_closing_to_opening(kind)) {
        LOU_VEC_POP(braces);
      }
      break;
    default:
      break;
  }
}

void lou_parser_skip_to(lou_parser_t *parser, lou_token_kind_t kind) {
  lou_token_kind_t *braces = LOU_MEMPOOL_VEC_NEW(parser->mempool, lou_token_kind_t);
  lou_token_t token;
  do {
    token = lou_parser_take(parser);
    lou_parser_brace_factor(&braces, token.kind);
  } while (token.kind != LOU_TOKEN_EOI && (lou_vec_len(braces) != 0 || token.kind == kind));
}

bool lou_parser_is_list_end(lou_parser_t *parser, lou_token_kind_t kind) {
  if (lou_parser_take_if(parser, LOU_TOKEN_COMMA)) {
    if (lou_parser_take_if(parser, kind)) {
      return true;
    }
    return false;
  } else if (lou_parser_take_if(parser, kind)) {
    return true;
  } else {
    lou_parser_err(parser, lou_parser_peek(parser).slice, "expected comma or closing brace");
    lou_parser_skip_to(parser, kind);
    return true;
  }
}

bool lou_parser_peek_n(lou_parser_t *parser, size_t length, const lou_token_kind_t *kinds) {
  lou_token_t *tokens = alloca(sizeof(lou_token_t) * length);
  bool result = true;
  for (size_t i = 0; i < length; i++) {
    tokens[i] = lou_parser_take(parser);
    if (kinds[i] != tokens[i].kind) {
      length = i + 1;
      result = false;
    }
  }
  for (size_t i = 0; i < length; i++) {
    lou_parser_queue(parser, tokens[i]);
  }
  return result;
}
