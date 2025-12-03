#pragma once

#include "lou/parser/ast/body.h"
#include "lou/sema/sema.h"
#include <stdbool.h>

void lou_sema_analyze_body(lou_sema_t *sema, lou_ast_body_t *body);
