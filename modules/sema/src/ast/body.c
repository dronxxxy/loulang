#include "body.h"
#include "ast/expr.h"
#include "ast/expr_ctx.h"
#include "ast/node.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "lou/core/vec.h"
#include "lou/hir/code.h"
#include "lou/parser/ast/stmt.h"
#include "type.h"
#include "value.h"

static inline void lou_sema_emit_stmt(lou_sema_t *sema, lou_ast_stmt_t *stmt) {
  switch (stmt->kind) {
    case LOU_AST_STMT_RETURN: {
      lou_sema_break_scope(sema, LOU_SEMA_SCOPE_BREAK_RETURN);
      lou_sema_type_t *returns = lou_sema_returns(sema);
      lou_sema_value_t *value = RET_ON_NULL(lou_sema_expr_analyze_runtime(sema, stmt->ret.value, lou_sema_expr_ctx_new_runtime(NULL), false));
      lou_sema_type_t *type = lou_sema_value_is_runtime(value);
      if (!lou_sema_type_eq(returns, type)) {
        lou_sema_err(sema, stmt->slice, "function expected to return #T but got expression of type #T", returns, type);
        return;
      }
      lou_sema_push_stmt(sema, stmt->slice, lou_hir_stmt_new_ret(sema->mempool, lou_sema_value_as_hir(sema->mempool, value)));
      return;
    }
    case LOU_AST_STMT_NODE: lou_sema_analyze_node(sema, stmt->node); return;
    case LOU_AST_STMT_IF: NOT_IMPLEMENTED;
  }
  UNREACHABLE();
}

lou_sema_scope_t *lou_sema_emit_body(lou_sema_t *sema, lou_ast_body_t *body) {
  lou_sema_push_scope(sema);
  for (size_t i = 0; i < lou_vec_len(body->stmts); i++) {
    if (lou_sema_current_scope(sema)->break_kind) {
      lou_sema_err(sema, lou_slice_union(body->stmts[i]->slice, (*LOU_VEC_LAST(body->stmts))->slice), "unreachable code");
      break;
    }
    lou_sema_emit_stmt(sema, body->stmts[i]);
    // TODO: control flow checks
  }
  return lou_sema_pop_scope(sema);
}
