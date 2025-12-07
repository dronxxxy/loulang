#pragma once

#include "impl.h"
#include "lou/parser/ast/body.h"

lou_sema_scope_t *lou_sema_emit_body(lou_sema_t *sema, lou_ast_body_t *body);
