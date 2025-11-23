#include "lou/lexer/token.h"
#include <stdio.h>

#define WRITE_TOKEN(NAME) \
    case TOKEN_##NAME: \
      fputs(#NAME, stream); \
      return; \

void token_write(const token_t *token, FILE *stream) {
  switch (token->kind) {
    WRITE_TOKEN(OPENING_CIRCLE_BRACE);
    WRITE_TOKEN(CLOSING_CIRCLE_BRACE);
    WRITE_TOKEN(OPENING_FIGURE_BRACE);
    WRITE_TOKEN(CLOSING_FIGURE_BRACE);
    WRITE_TOKEN(OPENING_SQUARE_BRACE);
    WRITE_TOKEN(CLOSING_SQUARE_BRACE);
    WRITE_TOKEN(COLON);
    WRITE_TOKEN(FUNCTION_RETURNS);
    WRITE_TOKEN(MINUS);
    WRITE_TOKEN(FUN);
    WRITE_TOKEN(IDENT);
    WRITE_TOKEN(EOI);
    WRITE_TOKEN(FAILED);
  }
  fputs("(invalid)", stream);
}

