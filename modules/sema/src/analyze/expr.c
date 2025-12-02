#include "expr.h"
#include "const.h"
#include "lou/core/assertions.h"
#include "lou/core/mempool.h"
#include "lou/core/vec.h"
#include "sema.h"
#include "value.h"

lou_sema_value_t *lou_sema_analyze_expr(lou_sema_t *sema, lou_ast_expr_t *expr) {
  switch (expr->kind) {
    case LOU_AST_EXPR_IDENT:
      return lou_sema_resolve(sema, expr->ident);
    case LOU_AST_EXPR_CALL: {
      bool valid = true;
      lou_sema_value_t *callable = VALID(lou_sema_analyze_expr(sema, expr->call.inner));
      lou_sema_value_t **args = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_value_t*);
      for (size_t i = 0; i < lou_vec_len(expr->call.args); i++) {
        *LOU_VEC_PUSH(&args) = VALID(lou_sema_analyze_expr(sema, expr->call.args[i]));
      }
      if (!valid) {
        return NULL;
      }
      if (callable->kind == LOU_SEMA_VALUE_PLUGIN) {
        lou_slice_t *arg_slices = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_slice_t);
        for (size_t i = 0; i < lou_vec_len(expr->call.args); i++) {
          *LOU_VEC_PUSH(&arg_slices) = expr->call.args[i]->slice;
        }
        return lou_sema_call_plugin(sema, callable->plugin, expr->call.inner->slice, arg_slices, args);
      }
      lou_sema_err(sema, expr->slice, "#v is not callable", callable);
      return NULL;
    }
    case LOU_AST_EXPR_INTEGER:
      return lou_sema_value_new_constant(sema->mempool, lou_sema_const_new_integer(sema->mempool, expr->integer));
    case LOU_AST_EXPR_STRING:
      return lou_sema_value_new_constant(sema->mempool, lou_sema_const_new_string(sema->mempool, expr->string));
    case LOU_AST_EXPR_ARRAY: {
      bool valid = true;
      lou_sema_value_t **elements = LOU_MEMPOOL_VEC_NEW(sema->mempool, lou_sema_value_t*);
      for (size_t i = 0; i < lou_vec_len(expr->array.values); i++) {
        *LOU_VEC_PUSH(&elements) = VALID(lou_sema_analyze_expr(sema, expr->array.values[i]));
      }
      if (!valid) {
        return NULL;
      }
      lou_sema_err(sema, expr->slice, "array initializers are NIY");
      return NULL;
    }
    case LOU_AST_EXPR_GET_IDENT: {
      lou_sema_value_t *inner = NOT_NULL(lou_sema_analyze_expr(sema, expr->call.inner));
      lou_sema_err(sema, expr->get_ident.ident, "cannot extract #S from #v", expr->get_ident.ident, inner);
      return NULL;
    }
    case LOU_AST_EXPR_FUNC:
      // TODO: remove, temporary
      return lou_sema_value_new_constant(sema->mempool, lou_sema_const_new_string(sema->mempool, expr->string));
      lou_sema_err(sema, expr->slice, "functions are NIY");
      return NULL;
  }
  UNREACHABLE();
}

