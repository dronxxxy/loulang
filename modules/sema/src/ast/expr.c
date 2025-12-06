#include "expr.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "plugin.h"
#include "type.h"
#include "value.h"

static inline lou_sema_value_t *lou_sema_expr_outline_internal(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  switch (expr->kind) {
    case LOU_AST_EXPR_CHAR: {
      lou_sema_type_t *u8 = lou_sema_type_new_int(sema->mempool, LOU_SEMA_INT_8, false);
      lou_sema_const_t *constant = lou_sema_const_new_int(sema->mempool, u8, expr->character);
      return lou_sema_value_new_const(sema->mempool, constant);
    }
    case LOU_AST_EXPR_INTEGER: {
      lou_sema_type_t *type = ctx.expectation && ctx.expectation->kind == LOU_SEMA_TYPE_INTEGER ?
        ctx.expectation : lou_sema_type_default_int(sema);
      lou_sema_const_t *constant = lou_sema_const_new_int(sema->mempool, type, expr->integer);
      return lou_sema_value_new_const(sema->mempool, constant);
    }
    case LOU_AST_EXPR_IDENT: return lou_sema_resolve_skeleton(sema, expr->ident);
    case LOU_AST_EXPR_CALL: {
      lou_sema_value_t *value = NOT_NULL(lou_sema_expr_analyze(sema, expr->call.inner, ctx));

      lou_sema_plugin_t *plugin = lou_sema_value_is_plugin(value);
      if (plugin) return plugin->outline(lou_sema_plugin_ctx_new(sema, expr->call.inner->slice, expr->call.args), ctx);

      lou_sema_err(sema, expr->call.inner->slice, "#V is not callable", value);
      return NULL;
    }
    case LOU_AST_EXPR_STRING:
    case LOU_AST_EXPR_FUNC:
    case LOU_AST_EXPR_GET_IDENT:
    case LOU_AST_EXPR_ARRAY:
      NOT_IMPLEMENTED;
  }
  UNREACHABLE();
}

lou_sema_value_t *lou_sema_expr_outline(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  return expr->sema_value = lou_sema_expr_outline_internal(sema, expr, ctx);
}

lou_sema_value_t *lou_sema_expr_finalize(lou_sema_t *sema, lou_ast_expr_t *expr) {
  switch (expr->kind) {
    case LOU_AST_EXPR_CHAR: case LOU_AST_EXPR_INTEGER: return expr->sema_value;
    case LOU_AST_EXPR_IDENT: return lou_sema_resolve(sema, expr->ident);
    case LOU_AST_EXPR_CALL: {
      lou_sema_value_t *callable = expr->call.inner->sema_value;

      lou_sema_plugin_t *plugin = lou_sema_value_is_plugin(callable);
      if (plugin) {
        if (!plugin->finalize(lou_sema_plugin_ctx_new(sema, expr->call.inner->slice, expr->call.args), expr->sema_value)) return NULL;
        return expr->sema_value;
      }

      UNREACHABLE();
    }
    case LOU_AST_EXPR_GET_IDENT:
    case LOU_AST_EXPR_FUNC:
    case LOU_AST_EXPR_STRING:
    case LOU_AST_EXPR_ARRAY:
      NOT_IMPLEMENTED;
  }
  UNREACHABLE();
}

lou_sema_value_t *lou_sema_expr_outline_runtime(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_value_t *value = NOT_NULL(lou_sema_expr_outline(sema, expr, ctx));
  if (!lou_sema_value_is_runtime(value)) {
    lou_sema_err(sema, expr->slice, "a runtime expression was expected but got #V", value);
  }
  return value;
}

lou_sema_value_t *lou_sema_expr_analyze(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_value_t *value = NOT_NULL(lou_sema_expr_outline(sema, expr, ctx));
  if (!lou_sema_expr_finalize(sema, expr)) return NULL;
  return value;
}

lou_sema_type_t *lou_sema_expr_outline_type(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_value_t *value = NOT_NULL(lou_sema_expr_outline(sema, expr, ctx));
  lou_sema_type_t *type = lou_sema_value_is_type(value);
  if (!type) {
    lou_sema_err(sema, expr->slice, "a type was expected but got #V", value);
  }
  return type;
}

lou_sema_type_t *lou_sema_expr_analyze_type(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_type_t *type = NOT_NULL(lou_sema_expr_outline_type(sema, expr, ctx));
  lou_sema_expr_finalize(sema, expr);
  return type;
}
