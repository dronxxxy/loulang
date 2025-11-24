#pragma once 

#include "lou/core/slice.h"

typedef struct lou_sema_t lou_sema_t;

lou_sema_t *lou_sema_new(lou_slice_t path);
void lou_sema_free(lou_sema_t *sema);

bool lou_sema_failed(const lou_sema_t *sema);

void lou_sema_read(lou_sema_t *sema);
void lou_sema_analyze(lou_sema_t *sema);

