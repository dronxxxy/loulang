#pragma once

#include "impl.h"
#include "lou/parser/ast/expr.h"
#include "value.h"
#include "type.h"
#include "expr_ctx.h"

lou_sema_value_t *lou_sema_expr_outline(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx);
lou_sema_value_t *lou_sema_expr_finalize(lou_sema_t *sema, lou_ast_expr_t *expr, bool weak);

lou_sema_type_t *lou_sema_expr_outline_type(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx);
lou_sema_value_t *lou_sema_expr_outline_runtime(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx);

lou_sema_value_t *lou_sema_expr_analyze(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx, bool weak);
lou_sema_type_t *lou_sema_expr_analyze_type(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx, bool weak);
lou_sema_value_t *lou_sema_expr_analyze_runtime(lou_sema_t *sema, lou_ast_expr_t *expr, lou_sema_expr_ctx_t ctx, bool weak);
