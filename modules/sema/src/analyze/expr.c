#include "expr.h"
#include "analyze/body.h"
#include "const.h"
#include "lou/core/assertions.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "lou/hir/code.h"
#include "lou/hir/func.h"
#include "scope.h"
#include "sema.h"
#include "type.h"
#include "value.h"
#include "plugin.h"

static inline lou_sema_type_t *lou_sema_infer_func_type(lou_sema_t *sema, lou_ast_expr_func_t *func) {
  lou_sema_type_func_arg_t *args = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_type_func_arg_t);
  for (size_t i = 0; i < lou_vec_len(func->args); i++) {
    if (!func->args[i].type) {
      lou_sema_err(sema, func->args[i].name, "cannot infer function argument type");
      return NULL;
    }
    *LOU_VEC_PUSH(&args) = (lou_sema_type_func_arg_t) {
      .name = (lou_opt_slice_t) { true, func->args[i].name},
      .type = lou_sema_analyze_type(sema, func->args[i].type),
    };
  }
  lou_sema_type_t *returns = func->returns ? NOT_NULL(lou_sema_analyze_type(sema, func->returns)) : NULL;
  return lou_sema_type_new_function(sema->mempool, args, returns);
}

static inline bool lou_sema_check_func_inferring(lou_sema_t *sema, lou_slice_t slice, lou_ast_expr_func_t *func, lou_sema_type_t *expectation) {
  if (lou_vec_len(expectation->func.args) != lou_vec_len(func->args)) {
    lou_sema_err(sema, slice, "expected #l arguments but got #l", lou_vec_len(expectation->func.args), lou_vec_len(func->args));
      return false;
  }
  for (size_t i = 0; i < lou_vec_len(func->args); i++) {
    if (!func->args[i].type) continue;
    lou_sema_type_t *type = lou_sema_analyze_type(sema, func->args[i].type);
    if (!type) continue;
    if (!lou_sema_types_eq(type, expectation->func.args[i].type)) {
      lou_sema_err(sema, func->args[i].type->slice, "expected argument of type #T but got argument of type #T", expectation->func.args[i].type, type);
      return false;
    }
  }
  lou_sema_type_t *returns = func->returns ? lou_sema_analyze_type(sema, func->returns) : NULL;
  if (returns && !lou_sema_types_eq(expectation->func.returns, returns)) {
    lou_sema_err(sema, slice, "expected return value of type #T but got value returning #T", expectation->func.returns, returns);
    return false;
  }
  return true;
}

lou_sema_value_t *lou_sema_analyze_func_expr(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_type_t *type = ctx.expectation && ctx.expectation->kind == LOU_SEMA_TYPE_FUNCTION ? ctx.expectation : NULL;

  if (!type) {
    type = NOT_NULL(lou_sema_infer_func_type(sema, &expr->func));
  } else {
    NOT_NULL(lou_sema_check_func_inferring(sema, expr->slice, &expr->func, type));
  }

  // TODO: push func args
  lou_hir_func_t *func = lou_hir_func_new(sema->mempool);
  lou_sema_push_scope_frame(sema, lou_sema_scope_frame_new(sema->mempool, type->func.returns));
  lou_sema_analyze_body(sema, expr->func.body);
  lou_sema_pop_scope_frame(sema);

  return lou_sema_value_new_constant(sema->mempool, lou_sema_const_new_func(sema->mempool, type, func));
}

lou_sema_value_t *lou_sema_analyze_get_ident_expr(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_value_t *inner = NOT_NULL(lou_sema_analyze_expr(sema, expr->call.inner, lou_sema_expr_ctx_nested(ctx, NULL)));
  lou_sema_err(sema, expr->get_ident.ident, "cannot extract #S from #v", expr->get_ident.ident, inner);
  return NULL;
}

lou_sema_value_t *lou_sema_analyze_array_expr(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  bool valid = true;
  lou_sema_value_t **elements = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_value_t*);
  for (size_t i = 0; i < lou_vec_len(expr->array.values); i++) {
    // TODO: expectation of array elements type
    *LOU_VEC_PUSH(&elements) = VALID(lou_sema_analyze_expr(sema, expr->array.values[i], lou_sema_expr_ctx_nested(ctx, NULL)));
  }
  if (!valid) {
    return NULL;
  }
  lou_sema_err(sema, expr->slice, "array initializers are NIY");
  return NULL;
}

lou_sema_value_t *lou_sema_analyze_string_expr(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  return lou_sema_value_new_constant(sema->mempool, lou_sema_const_new_string(sema->mempool, lou_sema_type_new_string(sema->mempool), expr->string));
}

lou_sema_value_t *lou_sema_analyze_int_expr(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_type_t *type = ctx.expectation && ctx.expectation->kind == LOU_SEMA_TYPE_INTEGER ? ctx.expectation : lou_sema_default_integer_type(sema);
  return lou_sema_value_new_constant(sema->mempool, lou_sema_const_new_integer(sema->mempool, type, expr->integer));
}

lou_sema_value_t *lou_sema_analyze_call_expr(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  bool valid = true;
  lou_sema_value_t *callable = VALID(lou_sema_analyze_expr(sema, expr->call.inner, lou_sema_expr_ctx_nested(ctx, NULL)));
  lou_sema_value_t **args = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_value_t*);
  lou_sema_plugin_t *plugin = valid ? lou_sema_value_is_plugin(callable) : NULL;
  for (size_t i = 0; i < lou_vec_len(expr->call.args); i++) {
    // TODO: expectation of function arg types if function
    lou_sema_type_t *expectation = NULL;
    // TODO: rich plugin arg type resolving
    if (plugin && i < lou_vec_len(plugin->args)) expectation = plugin->args[i] ? plugin->args[i] : ctx.expectation;
    *LOU_VEC_PUSH(&args) = VALID(lou_sema_analyze_expr(sema, expr->call.args[i], lou_sema_expr_ctx_nested(ctx, expectation)));
  }
  if (!valid) {
    return NULL;
  }
  if (plugin) {
    lou_slice_t *arg_slices = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_slice_t);
    for (size_t i = 0; i < lou_vec_len(expr->call.args); i++) {
      *LOU_VEC_PUSH(&arg_slices) = expr->call.args[i]->slice;
    }
    return lou_sema_call_plugin(sema, plugin, expr->call.inner->slice, arg_slices, args);
  }
  lou_sema_err(sema, expr->slice, "#v is not callable", callable);
  return NULL;
}

lou_sema_value_t *lou_sema_analyze_expr(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  switch (expr->kind) {
    case LOU_AST_EXPR_IDENT: return lou_sema_resolve(sema, expr->ident);
    case LOU_AST_EXPR_CALL: return lou_sema_analyze_call_expr(sema, expr, ctx);
    case LOU_AST_EXPR_INTEGER: return lou_sema_analyze_int_expr(sema, expr, ctx);
    case LOU_AST_EXPR_STRING: return lou_sema_analyze_string_expr(sema, expr, ctx);
    case LOU_AST_EXPR_ARRAY: return lou_sema_analyze_array_expr(sema, expr, ctx);
    case LOU_AST_EXPR_GET_IDENT: return lou_sema_analyze_get_ident_expr(sema, expr, ctx);
    case LOU_AST_EXPR_FUNC: return lou_sema_analyze_func_expr(sema, expr, ctx);
  }
  UNREACHABLE();
}

lou_sema_value_t *lou_sema_analyze_runtime_expr(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_value_t *value = NOT_NULL(lou_sema_analyze_expr(sema, expr, ctx));
  if (!lou_sema_value_is_runtime(value)) {
    lou_sema_err(sema, expr->slice, "expected runtime vaue got #v", value);
  }
  return value;
}

lou_sema_type_t *lou_sema_analyze_type(lou_sema_t *sema, lou_ast_expr_t *expr) {
  lou_sema_value_t *value = NOT_NULL(lou_sema_analyze_expr(sema, expr, lou_sema_expr_ctx_new(sema->mempool, NULL)));
  lou_sema_type_t *type = lou_sema_value_is_type(value);
  if (!type) {
    lou_sema_err(sema, expr->slice, "expected type got #v", value);
  }
  return type;
}

lou_sema_expr_ctx_t lou_sema_expr_ctx_nested(lou_sema_expr_ctx_t base, lou_sema_type_t *type) {
  base.expectation = type;
  return base;
}

lou_sema_expr_ctx_t lou_sema_expr_ctx_new(lou_mempool_t *mempool, lou_sema_type_t *expectation) {
  return (lou_sema_expr_ctx_t) {
    .expectation = expectation,
    .code = lou_hir_code_new(mempool),
  };
}

lou_sema_expr_ctx_t lou_sema_expr_ctx_nested(lou_sema_expr_ctx_t base, lou_sema_type_t *type);
