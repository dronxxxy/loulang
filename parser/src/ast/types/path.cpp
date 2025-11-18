#include <parser/ast/types/path.hpp>

using namespace parser;

AstPathType::AstPathType(AstPath path) : path(path) {}

bool AstPathType::operator==(const AstType &other) const {
  if (auto type = dynamic_cast<const AstPathType*>(&other)) {
    return this->path == type->path;
  }
  return false;
}

