#include "expr.h"
#include "ast/body.h"
#include "ast/expr_ctx.h"
#include "impl.h"
#include "lou/core/assertions.h"
#include "lou/core/mempool.h"
#include "lou/core/slice.h"
#include "lou/core/vec.h"
#include "lou/hir/code.h"
#include "lou/hir/func.h"
#include "lou/hir/hir.h"
#include "plugin.h"
#include "type.h"
#include "value.h"
#include <assert.h>

static inline lou_sema_type_t *lou_sema_infer_func_type(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  lou_sema_type_t **args = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_type_t*);
  for (size_t i = 0; i < lou_vec_len(expr->func.args); i++) {
    if (!expr->func.args[i].type) {
      lou_sema_err(sema, expr->func.args[i].name, "cannot infer function argument type");
      return NULL;
    }
    *LOU_VEC_PUSH(&args) = lou_sema_expr_analyze_type(sema, expr->func.args[i].type, lou_sema_expr_ctx_nested(ctx, NULL), true);
  }
  lou_sema_type_t *returns = expr->func.returns ?
    NOT_NULL(lou_sema_expr_analyze_type(sema, expr->func.returns, lou_sema_expr_ctx_nested(ctx, NULL), true)) : NULL;

  lou_sema_type_t *type = lou_sema_type_new_func(sema->mempool);
  lou_sema_type_init_func(type, args, returns);
  return type;
}

static inline bool lou_sema_check_func_inferring(lou_sema_t *sema, lou_slice_t slice, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  if (lou_vec_len(ctx.expectation->func.args) != lou_vec_len(expr->func.args)) {
    lou_sema_err(sema, slice, "expected #l arguments but got #l", lou_vec_len(ctx.expectation->func.args), lou_vec_len(expr->func.args));
    return false;
  }

  for (size_t i = 0; i < lou_vec_len(expr->func.args); i++) {
    if (!expr->func.args[i].type) continue;
    lou_sema_type_t *type = lou_sema_expr_analyze_type(sema, expr->func.args[i].type, lou_sema_expr_ctx_nested(ctx, NULL), false);

    if (!type) continue;
    if (!lou_sema_type_eq(type, ctx.expectation->func.args[i])) {
      lou_sema_err(sema, expr->func.args[i].type->slice, "expected argument of type #T but got argument of type #T", ctx.expectation->func.args[i], type);
      return false;
    }
  }
  lou_sema_type_t *returns = expr->func.returns ? lou_sema_expr_analyze_type(sema, expr->func.returns, lou_sema_expr_ctx_nested(ctx, NULL), false) : NULL;
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
      lou_sema_value_t *value = NOT_NULL(lou_sema_expr_analyze(sema, expr->call.inner, ctx, true));

      lou_sema_plugin_t *plugin = lou_sema_value_is_plugin(value);
      if (plugin) return plugin->outline(lou_sema_plugin_ctx_new(sema, expr->call.inner->slice, expr->call.args), ctx);

      if (plugin) {
        return plugin->outline(lou_sema_plugin_ctx_new(sema, expr->call.inner->slice, expr->call.args), ctx);
      }
      
      lou_sema_type_t *runtime = lou_sema_value_is_runtime(value);
      if (runtime && runtime->kind == LOU_SEMA_TYPE_FUNCTION) {
        lou_hir_local_t *output = runtime->func.returns ? NOT_NULL(lou_sema_add_local_final(sema, runtime->func.returns)) : NULL;

        if (lou_vec_len(runtime->func.args) != lou_vec_len(expr->call.args)) {
          lou_sema_err(sema, expr->call.inner->slice, "funtions accepts #l argument#s but #l #s passed",
            lou_vec_len(runtime->func.args), lou_vec_len(runtime->func.args) == 1 ? "" : "s",
            lou_vec_len(expr->call.args), lou_vec_len(expr->call.args) == 1 ? "was" : "were");
          return NULL;
        }

        lou_hir_value_t **args = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_hir_value_t*);
        for (size_t i = 0; i < lou_vec_len(expr->call.args); i++) {
          lou_ast_expr_t *ast_arg = expr->call.args[i];
          lou_sema_value_t *arg = NOT_NULL(lou_sema_expr_analyze_runtime(sema, ast_arg, lou_sema_expr_ctx_new_runtime(runtime->func.args[i]), false));
          lou_sema_type_t *type = lou_sema_value_is_runtime(arg);
          lou_sema_type_t *expected_type = runtime->func.args[i];
          if (!lou_sema_type_eq(type, expected_type)) {
            lou_sema_err(sema, ast_arg->slice, "value of type #T was expected #T was passed", expected_type, type);
          }
          *LOU_VEC_PUSH(&args) = lou_sema_value_as_hir(sema->mempool, arg);
        }

        lou_sema_push_stmt(sema, expr->slice, lou_hir_stmt_new_call(sema->mempool, output, lou_sema_value_as_hir(sema->mempool, value), args));
        // TODO: prevent void assignment
        return runtime->func.returns ?
          lou_sema_value_new_local(sema->mempool, LOU_SEMA_IMMUTABLE, runtime->func.returns, output, lou_sema_current_scope_stack(sema)) :
          NULL;
      }

      lou_sema_err(sema, expr->call.inner->slice, "#V is not callable", value);
      return NULL;
    }
    case LOU_AST_EXPR_STRING:
      switch (expr->string.kind) {
        case LOU_TOKEN_STRING_NORMAL: return lou_sema_value_new_const(sema->mempool, lou_sema_const_new_string(sema->mempool, expr->string.content));
        case LOU_TOKEN_STRING_C: return lou_sema_value_new_const(sema->mempool, lou_sema_const_new_cstring(sema->mempool, expr->string.content));
      }
      UNREACHABLE();
    case LOU_AST_EXPR_FUNC: {
      lou_sema_type_t *type = ctx.expectation;
      if (!type || type->kind != LOU_SEMA_TYPE_FUNCTION) {
        type = NOT_NULL(lou_sema_infer_func_type(sema, expr, ctx));
      } else {
        NOT_NULL(lou_sema_check_func_inferring(sema, expr->slice, expr, ctx));
      }
      lou_sema_const_t *constant = lou_sema_const_new_func(sema->mempool, type, lou_hir_func_add(sema->hir,
        lou_sema_type_as_hir(sema->mempool, type)));
      return lou_sema_value_new_const(sema->mempool, constant);
    }
    case LOU_AST_EXPR_SCOPE: return lou_sema_expr_outline(sema, expr->scope_inner, ctx);
    case LOU_AST_EXPR_GET_IDENT: case LOU_AST_EXPR_ARRAY: NOT_IMPLEMENTED;
    case LOU_AST_EXPR_ASSIGN: {
      lou_sema_value_t *to_value = NOT_NULL(lou_sema_expr_outline_runtime(sema, expr->assign.to, ctx));
      lou_sema_value_t *what_value = NOT_NULL(lou_sema_expr_outline_runtime(sema, expr->assign.what,
        lou_sema_expr_ctx_nested(ctx, lou_sema_value_is_runtime(to_value))));
      lou_sema_type_t *what = lou_sema_value_is_runtime(what_value);
      lou_sema_type_t *to = lou_sema_value_is_runtime(to_value);
      if (!lou_sema_type_eq(what, to)) {
        lou_sema_err(sema, expr->assign.what->slice, "value of type #T was expected but got value of type #T", to, what);
        return NULL;
      }
      lou_sema_value_local_t *local = lou_sema_value_is_local(to_value);
      if (!local || local->mutability != LOU_SEMA_MUTABLE) {
        lou_sema_err(sema, expr->assign.to->slice, "#V is not assignable", local);
        return NULL;
      }
      return what_value;
    }
    case LOU_AST_EXPR_BINOP: {
      lou_sema_type_t *left = lou_sema_value_is_runtime(NOT_NULL(lou_sema_expr_outline_runtime(sema, expr->binop.left, ctx)));
      lou_sema_type_t *right = lou_sema_value_is_runtime(NOT_NULL(lou_sema_expr_outline_runtime(sema, expr->binop.right, lou_sema_expr_ctx_new_runtime(left))));
      if (!lou_sema_type_eq(left, right)) {
        lou_sema_err(sema, expr->binop.right->slice, "binop operates with equal types only but trying to apply it to values of type #T != #T", left, right);
        return NULL;
      }
      lou_sema_type_t *type = left;
      switch (expr->binop.kind) {
        case LOU_AST_BINOP_ADD:
        case LOU_AST_BINOP_SUBTRACT:
        case LOU_AST_BINOP_MULTIPLY:
        case LOU_AST_BINOP_DIVIDE:
        case LOU_AST_BINOP_MOD:
        case LOU_AST_BINOP_SHORT_ADD:
        case LOU_AST_BINOP_SHORT_SUBTRACT:
        case LOU_AST_BINOP_SHORT_MULTIPLY:
        case LOU_AST_BINOP_SHORT_DIVIDE:
        case LOU_AST_BINOP_SHORT_MOD:
          break;

        case LOU_AST_BINOP_EQUALS:
        case LOU_AST_BINOP_NOT_EQUALS:
        case LOU_AST_BINOP_GREATER:
        case LOU_AST_BINOP_LESS:
        case LOU_AST_BINOP_GREATER_OR_EQUALS:
        case LOU_AST_BINOP_LESS_OR_EQUALS:
          type = lou_sema_type_new_bool(sema->mempool);
          break;
      }
      return lou_sema_value_new_local(sema->mempool, LOU_SEMA_IMMUTABLE, type, lou_sema_add_local_final(sema, type),
        lou_sema_current_scope_stack(sema));
    }
    case LOU_AST_EXPR_UNARY: {
      lou_sema_type_t *type = lou_sema_value_is_runtime(NOT_NULL(lou_sema_expr_outline_runtime(sema, expr->binop.left, ctx)));
      if (type->kind != LOU_SEMA_TYPE_INTEGER) {
        lou_sema_err(sema, expr->slice, "this unary operator can be applied to integers only but value has type #T", type);
        return NULL;
      }
      return lou_sema_value_new_local(sema->mempool, LOU_SEMA_IMMUTABLE, type, lou_sema_add_local_final(sema, type),
        lou_sema_current_scope_stack(sema));
    }
    case LOU_AST_EXPR_STRUCT_TYPE: {
      lou_sema_struct_field_t *fields = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_struct_field_t);
      for (size_t i = 0; i < lou_vec_len(expr->struct_type.fields); i++) {
        lou_ast_struct_field_t *field = &expr->struct_type.fields[i];
        for (size_t j = i + 1; j < lou_vec_len(expr->struct_type.fields); j++) {
          if (lou_slice_eq(field->name, expr->struct_type.fields[j].name)) {
            lou_sema_err(sema, expr->struct_type.fields[j].name, "field name duplicate");
          }
        }
        *LOU_VEC_PUSH(&fields) = lou_sema_struct_field_new(field->name, lou_sema_expr_outline_type(sema, field->type,
          lou_sema_expr_ctx_new_comptime()));
      }
      return lou_sema_value_new_type(sema->mempool, lou_sema_type_new_struct(sema->mempool, fields));
    }
  }
  UNREACHABLE();
}

lou_sema_value_t *lou_sema_expr_outline(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx) {
  return expr->sema_value = lou_sema_expr_outline_internal(sema, expr, ctx);
}

static inline lou_sema_value_t *lou_sema_analyze_binop_arithm_int(
  lou_sema_t *sema,
  lou_hir_binop_arithm_int_t kind,
  lou_sema_value_t *left,
  lou_sema_value_t *right,
  lou_ast_expr_t *expr
) {
  lou_hir_local_t *output = lou_sema_value_is_local(expr->sema_value)->hir;
  lou_sema_type_t *type = lou_sema_value_is_runtime(left);
  if (type->kind != LOU_SEMA_TYPE_INTEGER) {
    lou_sema_err(sema, expr->slice, "this binop can be applied to integers only");
    return NULL;
  }
  lou_sema_push_stmt(sema, expr->slice, lou_hir_stmt_new_binop_arithm_int(sema->mempool, kind, output, lou_sema_value_as_hir(sema->mempool, left),
    lou_sema_value_as_hir(sema->mempool, right)));
  return expr->sema_value;
}

static inline lou_sema_value_t *lou_sema_analyze_binop_arithm(
  lou_sema_t *sema,
  lou_hir_binop_arithm_t kind,
  lou_sema_value_t *left,
  lou_sema_value_t *right,
  lou_ast_expr_t *expr
) {
  lou_hir_local_t *output = lou_sema_value_is_local(expr->sema_value)->hir;
  lou_sema_type_t *type = lou_sema_value_is_runtime(left);
  lou_hir_binop_arithm_of_t of; 
  if (type->kind == LOU_SEMA_TYPE_INTEGER) {
    of = LOU_HIR_BINOP_ARITHM_OF_INT;
  } else {
    lou_sema_err(sema, expr->slice, "this binop can be applied to integers only");
    return NULL;
  }
  lou_sema_push_stmt(sema, expr->slice, lou_hir_stmt_new_binop_arithm(sema->mempool, kind, of, output, lou_sema_value_as_hir(sema->mempool, left),
    lou_sema_value_as_hir(sema->mempool, right)));
  return expr->sema_value;
}

static inline lou_sema_value_t *lou_sema_analyze_binop_short(lou_sema_t *sema, lou_sema_value_t *left, lou_sema_value_t *value, lou_ast_expr_t *expr) {
  lou_sema_value_local_t *output = lou_sema_value_is_local(left);
  if (!output || output->mutability != LOU_SEMA_MUTABLE) {
    lou_sema_err(sema, expr->slice, "#V is not assignable", left);
  }
  lou_sema_push_stmt(sema, expr->slice, lou_hir_stmt_new_store_var(sema->mempool, output->hir, lou_sema_value_as_hir(sema->mempool, value)));
  return expr->sema_value;
}

static inline lou_sema_value_t *lou_sema_analyze_binop_order(
  lou_sema_t *sema,
  lou_hir_binop_order_t kind,
  lou_sema_value_t *left,
  lou_sema_value_t *right,
  lou_ast_expr_t *expr
) {
  lou_hir_local_t *output = lou_sema_value_is_local(expr->sema_value)->hir;
  lou_sema_type_t *type = lou_sema_value_is_runtime(left);
  lou_hir_binop_order_of_t of;
  if (type->kind == LOU_SEMA_TYPE_INTEGER) {
    of = LOU_HIR_BINOP_ORDER_OF_INT;
  } else {
    lou_sema_err(sema, expr->slice, "this binop can be applied to integers only");
    return NULL;
  }
  lou_sema_push_stmt(sema, expr->slice, lou_hir_stmt_new_binop_order(sema->mempool, kind, of, output, lou_sema_value_as_hir(sema->mempool, left),
    lou_sema_value_as_hir(sema->mempool, right)));
  return expr->sema_value;
}

static inline lou_sema_value_t *lou_sema_analyze_binop_eq(
  lou_sema_t *sema,
  lou_hir_binop_eq_t kind,
  lou_sema_value_t *left,
  lou_sema_value_t *right,
  lou_ast_expr_t *expr
) {
  lou_hir_local_t *output = lou_sema_value_is_local(expr->sema_value)->hir;
  lou_sema_type_t *type = lou_sema_value_is_runtime(left);
  lou_hir_binop_eq_of_t of;
  if (type->kind == LOU_SEMA_TYPE_INTEGER) {
    of = LOU_HIR_BINOP_EQ_OF_INT;
  } else if (type->kind == LOU_SEMA_TYPE_BOOL) {
    of = LOU_HIR_BINOP_EQ_OF_BOOL;
  } else {
    lou_sema_err(sema, expr->slice, "this binop can be applied for type #T", type);
    return NULL;
  }
  lou_sema_push_stmt(sema, expr->slice, lou_hir_stmt_new_binop_eq(sema->mempool, kind, of, output, lou_sema_value_as_hir(sema->mempool, left),
    lou_sema_value_as_hir(sema->mempool, right)));
  return expr->sema_value;
}

lou_sema_value_t *lou_sema_expr_finalize(lou_sema_t *sema, lou_ast_expr_t *expr, bool weak) {
  switch (expr->kind) {
    case LOU_AST_EXPR_CHAR: case LOU_AST_EXPR_INTEGER: return expr->sema_value;
    case LOU_AST_EXPR_SCOPE: return lou_sema_expr_finalize(sema, expr->scope_inner, weak);
    case LOU_AST_EXPR_IDENT: {
      if (weak) return expr->sema_value;
      return lou_sema_resolve(sema, expr->ident);
    }
    case LOU_AST_EXPR_CALL: {
      // TODO: `outline` and `finalize` works with its context without sema_value in the AST
      lou_sema_value_t *callable = NOT_NULL(expr->call.inner->sema_value);

      lou_sema_plugin_t *plugin = lou_sema_value_is_plugin(callable);
      if (plugin && plugin->finalize) {
        NOT_NULL(plugin->finalize(lou_sema_plugin_ctx_new(sema, expr->call.inner->slice, expr->call.args), expr->sema_value));
      }

      return expr->sema_value;
    }
    case LOU_AST_EXPR_STRING: return expr->sema_value;
    case LOU_AST_EXPR_FUNC: {
      lou_sema_value_t *value = expr->sema_value;
      lou_sema_const_t *constant = lou_sema_value_is_const(value);
      lou_hir_func_t *func = constant->func;

      lou_sema_push_scope_stack(sema, constant->type->func.returns);
      lou_sema_push_scope(sema);
      for (size_t i = 0; i < lou_vec_len(expr->func.args); i++) {
        lou_ast_expr_func_arg_t *arg = &expr->func.args[i];
        lou_sema_type_t *type = constant->type->func.args[i];
        lou_hir_local_t *local = arg->is_var ? lou_sema_add_local_var(sema, type) : lou_sema_add_local_final(sema, type);
        lou_sema_mutability_t mutability = arg->is_var ? LOU_SEMA_MUTABLE : LOU_SEMA_IMMUTABLE;
        lou_sema_add_full_decl(sema, arg->name, lou_sema_value_new_local(sema->mempool, mutability, type, local, lou_sema_current_scope_stack(sema)));
        lou_sema_push_stmt(sema, expr->slice, lou_hir_stmt_new_arg(sema->mempool, local, i));
      }
      lou_sema_scope_t *scope = lou_sema_emit_body(sema, expr->func.body);
      lou_sema_break_scope(sema, scope->break_kind);
      lou_sema_push_stmt(sema, expr->slice, lou_hir_stmt_new_code(sema->mempool, scope->code));
      lou_sema_scope_t *full_scope = lou_sema_pop_scope(sema);
      lou_sema_pop_scope_stack(sema);

      if (scope->break_kind != LOU_SEMA_SCOPE_BREAK_RETURN) {
        if (constant->type->func.returns) lou_sema_err(sema, expr->slice, "function returns a value so there is should be return statement");
        lou_hir_code_append_stmt(scope->code, lou_hir_stmt_new_ret(sema->mempool, NULL));
      }
      lou_hir_func_init(func, full_scope->code);
      return value;
    }
    case LOU_AST_EXPR_GET_IDENT:
    case LOU_AST_EXPR_ARRAY:
      NOT_IMPLEMENTED;
    case LOU_AST_EXPR_ASSIGN: {
      lou_sema_value_t *what = NOT_NULL(lou_sema_expr_finalize(sema, expr->assign.what, false));
      lou_sema_value_local_t *to = lou_sema_value_is_local(NOT_NULL(lou_sema_expr_finalize(sema, expr->assign.to, false)));
      lou_sema_push_stmt(sema, expr->slice, lou_hir_stmt_new_store_var(sema->mempool, to->hir, lou_sema_value_as_hir(sema->mempool, what)));
      return what;
    }
    case LOU_AST_EXPR_BINOP: {
      lou_sema_value_t *left = NOT_NULL(lou_sema_expr_finalize(sema, expr->binop.left, false));
      lou_sema_value_t *right = NOT_NULL(lou_sema_expr_finalize(sema, expr->binop.right, false));
      switch (expr->binop.kind) {
        case LOU_AST_BINOP_ADD: return lou_sema_analyze_binop_arithm(sema, LOU_HIR_BINOP_ARITHM_ADD, left, right, expr);
        case LOU_AST_BINOP_SUBTRACT: return lou_sema_analyze_binop_arithm(sema, LOU_HIR_BINOP_ARITHM_SUBTRACT, left, right, expr);
        case LOU_AST_BINOP_MULTIPLY: return lou_sema_analyze_binop_arithm(sema, LOU_HIR_BINOP_ARITHM_MULTIPLY, left, right, expr);
        case LOU_AST_BINOP_DIVIDE: return lou_sema_analyze_binop_arithm(sema, LOU_HIR_BINOP_ARITHM_DIVIDE, left, right, expr);
        case LOU_AST_BINOP_MOD: return lou_sema_analyze_binop_arithm_int(sema, LOU_HIR_BINOP_ARITHM_INT_MOD, left, right, expr);

        case LOU_AST_BINOP_SHORT_ADD: return lou_sema_analyze_binop_short(sema, left, lou_sema_analyze_binop_arithm(sema, LOU_HIR_BINOP_ARITHM_ADD, left, right, expr), expr);
        case LOU_AST_BINOP_SHORT_SUBTRACT: return lou_sema_analyze_binop_short(sema, left, lou_sema_analyze_binop_arithm(sema, LOU_HIR_BINOP_ARITHM_SUBTRACT, left, right, expr), expr);
        case LOU_AST_BINOP_SHORT_MULTIPLY: return lou_sema_analyze_binop_short(sema, left, lou_sema_analyze_binop_arithm(sema, LOU_HIR_BINOP_ARITHM_MULTIPLY, left, right, expr), expr);
        case LOU_AST_BINOP_SHORT_DIVIDE: return lou_sema_analyze_binop_short(sema, left, lou_sema_analyze_binop_arithm(sema, LOU_HIR_BINOP_ARITHM_DIVIDE, left, right, expr), expr);
        case LOU_AST_BINOP_SHORT_MOD: return lou_sema_analyze_binop_short(sema, left, lou_sema_analyze_binop_arithm_int(sema, LOU_HIR_BINOP_ARITHM_INT_MOD, left, right, expr), expr);

        case LOU_AST_BINOP_EQUALS: return lou_sema_analyze_binop_eq(sema, LOU_HIR_BINOP_EQUALS, left, right, expr);
        case LOU_AST_BINOP_NOT_EQUALS: return lou_sema_analyze_binop_eq(sema, LOU_HIR_BINOP_NOT_EQUALS, left, right, expr);

        case LOU_AST_BINOP_GREATER: return lou_sema_analyze_binop_order(sema, LOU_HIR_BINOP_ORDER_GREATER, left, right, expr);
        case LOU_AST_BINOP_LESS: return lou_sema_analyze_binop_order(sema, LOU_HIR_BINOP_ORDER_LESS, left, right, expr);
        case LOU_AST_BINOP_GREATER_OR_EQUALS: return lou_sema_analyze_binop_order(sema, LOU_HIR_BINOP_ORDER_GREATER_OR_EQUALS, left, right, expr);
        case LOU_AST_BINOP_LESS_OR_EQUALS: return lou_sema_analyze_binop_order(sema, LOU_HIR_BINOP_ORDER_LESS_OR_EQUALS, left, right, expr);
      }
      UNREACHABLE();
    }
    case LOU_AST_EXPR_UNARY: {
      lou_sema_value_t *value = NOT_NULL(lou_sema_expr_finalize(sema, expr->unary.inner, false));
      lou_sema_value_local_t *to = lou_sema_value_is_local(expr->sema_value);
      lou_sema_push_stmt(sema, expr->slice, lou_hir_stmt_negative_int(sema->mempool, to->hir, lou_sema_value_as_hir(sema->mempool, value)));
      return expr->sema_value;
    }
    case LOU_AST_EXPR_STRUCT_TYPE: {
      return expr->sema_value;
    }
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
  lou_sema_value_t *finalized = NOT_NULL(lou_sema_expr_finalize(sema, expr, weak));
  assert(value == finalized);
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
  NOT_NULL(lou_sema_expr_finalize(sema, expr, weak));
  return type;
}

lou_sema_value_t *lou_sema_expr_analyze_runtime(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx, bool weak) {
  lou_sema_value_t *type = NOT_NULL(lou_sema_expr_outline_runtime(sema, expr, ctx));
  NOT_NULL(lou_sema_expr_finalize(sema, expr, weak));
  return type;
}
