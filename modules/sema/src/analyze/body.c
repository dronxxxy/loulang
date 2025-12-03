#include "body.h"
#include "scope.h"
#include "sema.h"

lou_sema_scope_t *lou_sema_analyze_body(lou_sema_t *sema, lou_ast_body_t *body) {
  lou_sema_push_scope(sema);
  return lou_sema_pop_scope(sema);
}

