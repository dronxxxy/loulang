#include "body.h"
#include "lou/core/assertions.h"
#include "lou/core/vec.h"
#include "lou/parser/ast/stmt.h"

static inline void lou_sema_emit_stmt(lou_sema_t *sema, lou_ast_stmt_t *stmt) {
  switch (stmt->kind) {
    case LOU_AST_STMT_RETURN:
    case LOU_AST_STMT_EXPR:
    case LOU_AST_STMT_NODE:
    case LOU_AST_STMT_IF:
      NOT_IMPLEMENTED;
  }
  UNREACHABLE();
}

lou_sema_scope_t *lou_sema_emit_body(lou_sema_t *sema, lou_ast_body_t *body) {
  lou_sema_push_scope(sema);
  for (size_t i = 0; i < lou_vec_len(body->stmts); i++) {
    lou_sema_emit_stmt(sema, body->stmts[i]);
    // TODO: control flow checks
  }
  return lou_sema_pop_scope(sema);
}
