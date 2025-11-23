#pragma once

#include "token.h"
#include <stdbool.h>

typedef struct lexer_t lexer_t;

lexer_t *lexer_new(lou_slice_t path);
void lexer_free(lexer_t *lexer);

bool lexer_failed(const lexer_t *lexer);
token_t lexer_next(lexer_t *lexer);
