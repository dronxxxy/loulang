#pragma once

#include "type.h"

typedef struct {

} lou_sema_value_t;

lou_sema_type_t *lou_sema_value_is_runtime(lou_sema_value_t *value);
lou_sema_type_t *lou_sema_value_is_type(lou_sema_value_t *value);
