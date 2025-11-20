#pragma once

#include <lexer/token.hpp>
#include <functional>

namespace lexer {
  using TokenDataFilter = std::function<bool (const TokenData&)>;

  namespace filters {
    TokenDataFilter isSimple(SimpleTokenData target);
  }
}
