#pragma once

#include "lexer/token.h"
#include <stdbool.h>

typedef struct lexer_t lexer_t;

lexer_t *lexer_new(slice_t path);
void lexer_free(lexer_t *lexer);

bool lexer_failed(const lexer_t *lexer);
token_t lexer_next(lexer_t *lexer);
