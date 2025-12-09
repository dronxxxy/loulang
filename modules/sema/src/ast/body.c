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
    case LOU_AST_STMT_IF: {
      lou_sema_type_t *boolean = lou_sema_type_new_bool(sema->mempool);
      lou_sema_value_t *value = RET_ON_NULL(lou_sema_expr_analyze_runtime(sema, stmt->if_else.condition, lou_sema_expr_ctx_new_runtime(boolean), false));
      lou_sema_type_t *type = lou_sema_value_is_runtime(value);
      if (!lou_sema_type_eq(type, boolean)) {
        lou_sema_err(sema, stmt->if_else.condition->slice, "a boolean value was expected but got #T", type);
      }
      lou_sema_scope_t *scope = lou_sema_emit_body(sema, stmt->if_else.body);
      lou_sema_scope_t *else_scope = stmt->if_else.else_body ? lou_sema_emit_body(sema, stmt->if_else.else_body) : NULL;
      if (scope->break_kind && else_scope->break_kind) {
        lou_sema_break_scope(sema, else_scope->break_kind < scope->break_kind ? else_scope->break_kind : scope->break_kind);
      }
      lou_sema_push_stmt(sema, stmt->slice, lou_hir_stmt_new_cond(sema->mempool, lou_sema_value_as_hir(sema->mempool, value),
        scope->code,
        else_scope ? else_scope->code : NULL
      ));
      return;
    }
    case LOU_AST_STMT_WHILE: {
      lou_sema_push_scope(sema);

      lou_sema_type_t *boolean = lou_sema_type_new_bool(sema->mempool);
      lou_sema_value_t *value = RET_ON_NULL(lou_sema_expr_analyze_runtime(sema, stmt->while_loop.condition,
        lou_sema_expr_ctx_new_runtime(boolean), false));
      lou_sema_type_t *type = lou_sema_value_is_runtime(value);
      if (!lou_sema_type_eq(type, boolean)) {
        lou_sema_err(sema, stmt->while_loop.condition->slice, "a boolean value was expected but got #T", type);
      }

      lou_hir_stmt_t *loop_stmt = lou_hir_stmt_new_loop(sema->mempool, lou_sema_value_as_hir(sema->mempool, value),
        lou_sema_current_scope(sema)->code);

      lou_sema_scope_t *scope = lou_sema_emit_body(sema, stmt->while_loop.body);
      if (scope->break_kind) {
        lou_sema_break_scope(sema, scope->break_kind);
      }

      lou_hir_code_t *break_code = lou_hir_code_new(sema->mempool);
      lou_hir_code_append_stmt(break_code, lou_hir_stmt_new_break(sema->mempool, loop_stmt));
      lou_sema_push_stmt(sema, stmt->slice, lou_hir_stmt_new_cond(sema->mempool, lou_sema_value_as_hir(sema->mempool, value),
        scope->code, break_code));
      lou_sema_pop_scope(sema);
      lou_sema_push_stmt(sema, stmt->slice, loop_stmt);
      return;
    }
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
