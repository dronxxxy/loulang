#pragma once

#include <optional>

#define TRY(VALUE) \
  ({ \
    auto __value = VALUE; \
    if (!__value) return std::nullopt; \
    __value.value(); \
  })


