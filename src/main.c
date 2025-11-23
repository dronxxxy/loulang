#include "core/log.h"
#include "lexer/lexer.h"
#include "log.h"

int main(int argc, char** argv) {
  log_init();

  lexer_t *lexer = lexer_create("./examples/test/lexer.lou");
  if (!lexer) {
    return 1;
  }

  for (token_t token = lexer_next(lexer); token.kind != TOKEN_EOI; token = lexer_next(lexer)) {
    log_fmt(LOG_DEBUG, "token: #T", &token); 
  }

  int status = lexer_failed(lexer) ? 1 : 0;

  lexer_free(lexer);
  return status;
}
