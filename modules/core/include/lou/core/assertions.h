#pragma once

#include <assert.h>

#define UNREACHABLE __builtin_unreachable
#define NOT_IMPLEMENTED assert(0 && "not implemented yet!")
#define NOT_NULL(VALUE) ({ typeof(VALUE) __value = VALUE; if (!__value) return NULL; __value; })
#define RET_ON_NULL(VALUE) ({ typeof(VALUE) __value = VALUE; if (!__value) return; __value; })
#define IF_NULL_SET(VALUE, WHAT) ({ typeof(VALUE) __value = VALUE; if (!__value) WHAT = false; __value; })
#define VALID(VALUE) IF_NULL_SET(VALUE, valid)
