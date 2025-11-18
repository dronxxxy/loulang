#pragma once

#include <string_view>
#include <variant>
#include <vector>

namespace parser {
  class AstPathIdentSegment {
  private:
    std::string_view value;

  public:
    AstPathIdentSegment(std::string_view value);
    bool operator==(const AstPathIdentSegment &other) const;
  };

  typedef std::variant<AstPathIdentSegment> AstPathSegment;

  class AstPath {
  private:
    std::vector<AstPathSegment> segments;

  public:
    AstPath(std::vector<AstPathSegment> &&segments);
    bool operator==(const AstPath &other) const;
  };
}

