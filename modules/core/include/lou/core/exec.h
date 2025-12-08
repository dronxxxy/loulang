#pragma once

#include <stddef.h>
#include <stdbool.h>

bool lou_exec(const char *binary, char *const *args, size_t length, int *status);
