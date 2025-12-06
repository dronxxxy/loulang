#pragma once

#include "analyze/expr_ctx.h"
#include "lou/parser/ast/expr.h"
#include "lou/sema/sema.h"

typedef struct lou_sema_type_t lou_sema_type_t;
typedef struct lou_sema_value_t lou_sema_value_t;

lou_sema_value_t *lou_sema_analyze_expr(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx);
lou_sema_value_t *lou_sema_analyze_runtime_expr(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx);
lou_sema_type_t *lou_sema_analyze_type(lou_sema_t *sema, lou_ast_expr_t *expr, bool weak);

