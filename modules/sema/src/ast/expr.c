#include "expr.h"
#include "ast/body.h"
#include "ast/expr_ctx.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "lou/core/vec.h"
#include "lou/hir/code.h"
#include "lou/hir/func.h"
#include "lou/hir/hir.h"
#include "plugin.h"
#include "type.h"
#include "value.h"

static inline lou_sema_type_t *lou_sema_infer_func_type(lou_sema_t *sema, lou_ast_expr_func_t *func, lou_sema_expr_ctx_t ctx) {
  lou_sema_type_t **args = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_type_t*);
  for (size_t i = 0; i < lou_vec_len(func->args); i++) {
    if (!func->args[i].type) {
      lou_sema_err(sema, func->args[i].name, "cannot infer function argument type");
      return NULL;
    }
    *LOU_VEC_PUSH(&args) = lou_sema_expr_analyze_type(sema, func->args[i].type, lou_sema_expr_ctx_nested(ctx, NULL), true);
  }
  lou_sema_type_t *returns = func->returns ?
    NOT_NULL(lou_sema_expr_analyze_type(sema, func->returns, lou_sema_expr_ctx_nested(ctx, NULL), true)) : NULL;

  lou_sema_type_t *type = lou_sema_type_new_func(sema->mempool);
  lou_sema_type_init_func(type, args, returns);
  return type;
}

static inline bool lou_sema_check_func_inferring(lou_sema_t *sema, lou_slice_t slice, lou_ast_expr_func_t *func, lou_sema_expr_ctx_t ctx) {
  if (lou_vec_len(ctx.expectation->func.args) != lou_vec_len(func->args)) {
    lou_sema_err(sema, slice, "expected #l arguments but got #l", lou_vec_len(ctx.expectation->func.args), lou_vec_len(func->args));
    return false;
  }

  for (size_t i = 0; i < lou_vec_len(func->args); i++) {
    if (!func->args[i].type) continue;
    lou_sema_type_t *type = lou_sema_expr_analyze_type(sema, func->args[i].type, lou_sema_expr_ctx_nested(ctx, NULL), false);

    if (!type) continue;
    if (!lou_sema_type_eq(type, ctx.expectation->func.args[i])) {
      lou_sema_err(sema, func->args[i].type->slice, "expected argument of type #T but got argument of type #T", ctx.expectation->func.args[i], type);
      return false;
    }
  }
  lou_sema_type_t *returns = func->returns ? lou_sema_expr_analyze_type(sema, func->returns, lou_sema_expr_ctx_nested(ctx, NULL), false) : NULL;
  if (returns && !lou_sema_type_eq(ctx.expectation->func.returns, returns)) {
    lou_sema_err(sema, slice, "expected return value of type #T but got value returning #T", ctx.expectation->func.returns, returns);
    return false;
  }
  return true;
}

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
      lou_sema_value_t *value = NOT_NULL(lou_sema_expr_analyze(sema, expr->call.inner, ctx, false));

      lou_sema_plugin_t *plugin = lou_sema_value_is_plugin(value);
      if (plugin) return plugin->outline(lou_sema_plugin_ctx_new(sema, expr->call.inner->slice, expr->call.args), ctx);

      lou_sema_err(sema, expr->call.inner->slice, "#V is not callable", value);
      return NULL;
    }
    case LOU_AST_EXPR_STRING:
      switch (expr->string.kind) {
        case LOU_TOKEN_STRING_NORMAL: return lou_sema_value_new_const(sema->mempool, lou_sema_const_new_string(sema->mempool, expr->string.content));
        case LOU_TOKEN_STRING_C: NOT_IMPLEMENTED;
      }
      UNREACHABLE();
    case LOU_AST_EXPR_FUNC: {
      lou_sema_type_t *type = ctx.expectation;
      if (!type || type->kind != LOU_SEMA_TYPE_FUNCTION) {
        type = NOT_NULL(lou_sema_infer_func_type(sema, &expr->func, ctx));
      } else {
        NOT_NULL(lou_sema_check_func_inferring(sema, expr->slice, &expr->func, ctx));
      }
      lou_sema_const_t *constant = lou_sema_const_new_func(sema->mempool, type,
        lou_hir_func_add(sema->hir, lou_sema_type_as_hir(sema->mempool, type)));
      return lou_sema_value_new_const(sema->mempool, constant);
    }
    case LOU_AST_EXPR_GET_IDENT:
    case LOU_AST_EXPR_ARRAY:
      NOT_IMPLEMENTED;
  }
  UNREACHABLE();
}

lou_sema_value_t *lou_sema_expr_outline(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  return expr->sema_value = lou_sema_expr_outline_internal(sema, expr, ctx);
}

lou_sema_value_t *lou_sema_expr_finalize(lou_sema_t *sema, lou_ast_expr_t *expr, bool weak) {
  switch (expr->kind) {
    case LOU_AST_EXPR_CHAR: case LOU_AST_EXPR_INTEGER: return expr->sema_value;
    case LOU_AST_EXPR_IDENT:
      if (weak) return expr->sema_value;
      return lou_sema_resolve(sema, expr->ident);
    case LOU_AST_EXPR_CALL: {
      lou_sema_value_t *callable = expr->call.inner->sema_value;

      lou_sema_plugin_t *plugin = lou_sema_value_is_plugin(callable);
      if (plugin) {
        if (!plugin->finalize(lou_sema_plugin_ctx_new(sema, expr->call.inner->slice, expr->call.args), expr->sema_value)) return NULL;
        return expr->sema_value;
      }

      UNREACHABLE();
    }
    case LOU_AST_EXPR_STRING: return expr->sema_value;
    case LOU_AST_EXPR_FUNC: {
      lou_sema_value_t *value = expr->sema_value;
      lou_sema_const_t *constant = lou_sema_value_is_const(value);
      lou_hir_func_t *func = constant->func;
      lou_sema_push_scope_stack(sema, constant->type->func.returns);
      lou_hir_func_init(func, lou_sema_emit_body(sema, expr->func.body)->code);
      lou_sema_pop_scope_stack(sema);
      return value;
    }
    case LOU_AST_EXPR_GET_IDENT:
    case LOU_AST_EXPR_ARRAY:
      NOT_IMPLEMENTED;
  }
  UNREACHABLE();
}

lou_sema_value_t *lou_sema_expr_outline_runtime(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_value_t *value = NOT_NULL(lou_sema_expr_outline(sema, expr, ctx));
  if (!lou_sema_value_is_runtime(value)) {
    lou_sema_err(sema, expr->slice, "a runtime expression was expected but got #V", value);
    return NULL;
  }
  return value;
}

lou_sema_value_t *lou_sema_expr_analyze(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx, bool weak) {
  lou_sema_value_t *value = NOT_NULL(lou_sema_expr_outline(sema, expr, ctx));
  if (!lou_sema_expr_finalize(sema, expr, weak)) return NULL;
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

lou_sema_type_t *lou_sema_expr_analyze_type(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx, bool weak) {
  lou_sema_type_t *type = NOT_NULL(lou_sema_expr_outline_type(sema, expr, ctx));
  lou_sema_expr_finalize(sema, expr, weak);
  return type;
}
