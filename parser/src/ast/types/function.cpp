#include <parser/ast/types/function.hpp>

using namespace parser;

bool AstFunctionTypeArg::operator==(const AstFunctionTypeArg &other) const {
  return 
    this->name == other.name &&
    *this->type == *other.type;
}

bool AstFunctionType::operator==(const AstType &other) const {
  if (auto type = dynamic_cast<const AstFunctionType*>(&other)) {
    return 
      this->args == type->args &&
      *this->returns == *type->returns;
  }
  return false;
}

AstFunctionType::AstFunctionType(
  std::optional<AstTypePtr> returns,
  std::vector<AstFunctionTypeArg> args
) : returns(returns), args(args) {}
