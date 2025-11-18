#include <parser/ast/path.hpp>

using namespace parser;

AstPathIdentSegment::AstPathIdentSegment(std::string_view value) :
  value(value) {}

bool AstPathIdentSegment::operator==(const AstPathIdentSegment &other) const {
  return this->value == other.value;
}

AstPath::AstPath(std::vector<AstPathSegment> &&segments) :
  segments(std::move(segments)) {}

bool AstPath::operator==(const AstPath &other) const {
  return this->segments == other.segments;
}
