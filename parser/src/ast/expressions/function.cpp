#include <parser/ast/expressions/function.hpp>

using namespace parser;

AstFunctionExpression::AstFunctionExpression(
  AstFunctionTypePtr type,
  AstBody body
) : type(type), body(body) {}

bool AstFunctionExpression::operator==(const AstExpression &other) const {
  if (auto expr = dynamic_cast<const AstFunctionExpression*>(&other)) {
    return 
      this->body == expr->body &&
      this->type == expr->type;
  }
  return false;
}
