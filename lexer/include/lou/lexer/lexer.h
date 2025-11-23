#pragma once

#include "token.h"
#include <stdbool.h>

typedef struct lou_lexer_t lou_lexer_t;

lou_lexer_t *lou_lexer_new(lou_slice_t path);
void lou_lexer_free(lou_lexer_t *lexer);

void lou_lexer_log_error(const lou_lexer_t *lexer, lou_slice_t slice, const char *fmt, va_list list); 
bool lou_lexer_failed(const lou_lexer_t *lexer);
lou_token_t lou_lexer_next(lou_lexer_t *lexer);
