#include <lexer/filters.hpp>
#include <variant>

using namespace lexer;
using namespace lexer::filters;

TokenDataFilter isSimple(SimpleTokenData target) {
  return [target](const TokenData &data) {
    if (auto simple = std::get_if<SimpleTokenData>(&data)) {
      return *simple == target;
    }
    return false;
  };
}
