#pragma once

#include <assert.h>

#define UNREACHABLE __builtin_unreachable
#define NOT_IMPLEMENTED assert(0 && "not implemented yet!")
