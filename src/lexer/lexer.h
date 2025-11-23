#pragma once

typedef struct lexer_t lexer_t;

lexer_t *lexer_create(const char *path);
void lexer_free(lexer_t *lexer);
