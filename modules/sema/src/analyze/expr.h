#pragma once

#include "lou/parser/ast/expr.h"
#include "lou/sema/sema.h"
#include "value.h"

lou_sema_value_t *lou_sema_analyze_expr(lou_sema_t *sema, lou_ast_expr_t *expr);

