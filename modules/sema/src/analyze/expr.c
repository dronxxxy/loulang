#include "expr.h"
#include "lou/core/assertions.h"
#include "lou/core/vec.h"
#include "sema.h"

lou_sema_value_t *lou_sema_analyze_expr(lou_sema_t *sema, lou_ast_expr_t *expr) {
  switch (expr->kind) {
    case LOU_AST_EXPR_IDENT:
      return lou_sema_resolve(sema, expr->ident);
    case LOU_AST_EXPR_CALL: {
      lou_sema_analyze_expr(sema, expr->call.inner);
      for (size_t i = 0; i < lou_vec_len(expr->call.args); i++) {
        lou_sema_analyze_expr(sema, expr->call.args[i]);
      }
      NOT_IMPLEMENTED;
      // inside a code
    }
    case LOU_AST_EXPR_FUNC:
      // read
    case LOU_AST_EXPR_INTEGER:
      // constant integer
    case LOU_AST_EXPR_GET_IDENT:
    case LOU_AST_EXPR_STRING:
    case LOU_AST_EXPR_ARRAY:
      NOT_IMPLEMENTED;
  }
  UNREACHABLE();
}

