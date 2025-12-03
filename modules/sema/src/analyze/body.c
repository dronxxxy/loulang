#include "body.h"
#include "analyze/expr.h"
#include "analyze/value.h"
#include "lou/core/assertions.h"
#include "lou/core/vec.h"
#include "lou/hir/code.h"
#include "lou/parser/ast/stmt.h"
#include "scope.h"
#include "sema.h"
#include "type.h"
#include "value.h"
#include <assert.h>

static inline void lou_sema_analyze_stmt(lou_sema_t *sema, lou_ast_stmt_t *stmt) {
  switch (stmt->kind) {
    case LOU_AST_STMT_RETURN: {
      lou_sema_value_t *value = stmt->ret.value ?
        lou_sema_analyze_runtime_expr(sema, stmt->ret.value, lou_sema_expr_ctx_new(sema->mempool, lou_sema_func_returns(sema))) :
        NULL;
      lou_sema_type_t *returns = lou_sema_func_returns(sema);
      lou_sema_type_t *type = value ? lou_sema_value_is_runtime(value) : NULL;
      if (!lou_sema_types_eq(returns, type)) {
        lou_sema_err(sema, stmt->slice, "function expects to return #T but #T were passed", returns, type);
        return;
      }
      lou_sema_push_stmt(sema, lou_hir_stmt_new_ret(sema->mempool, value ? lou_sema_emit_value(sema->mempool, value) : NULL));
      return;
    }
    case LOU_AST_STMT_EXPR:
      lou_sema_analyze_expr(sema, stmt->expr, lou_sema_expr_ctx_new(sema->mempool, NULL));
      return;
    case LOU_AST_STMT_NODE:
    case LOU_AST_STMT_IF: NOT_IMPLEMENTED;
  }
  UNREACHABLE();
}

void lou_sema_analyze_body(lou_sema_t *sema, lou_ast_body_t *body) {
  lou_sema_push_scope(sema);
  for (size_t i = 0; i < lou_vec_len(body->stmts); i++) {
    lou_sema_analyze_stmt(sema, body->stmts[i]);
    // TODO: control flow check;
  }
  lou_sema_pop_scope(sema);
}

