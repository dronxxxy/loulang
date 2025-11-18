#include <parser/ast/body.hpp>

using namespace parser;

AstBody::AstBody(std::vector<AstBodyChild> &&children) :
  children(std::move(children)) {}

bool AstBody::operator==(const AstBody &other) const {
  return this->children == other.children;
}
