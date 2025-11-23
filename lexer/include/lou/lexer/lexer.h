#pragma once

#include "token.h"
#include <stdbool.h>

typedef struct lou_lexer_t lou_lexer_t;

lou_lexer_t *lou_lexer_new(lou_slice_t path);
void lou_lexer_free(lou_lexer_t *lexer);

bool lou_lexer_failed(const lou_lexer_t *lexer);
lou_token_t lou_lexer_next(lou_lexer_t *lexer);
