#include <parser/ast/nodes/decl.hpp>

using namespace parser;

AstDeclNode::AstDeclNode(
  AstDeclKind kind,
  std::optional<std::string> externName,
  std::optional<AstExpressionPtr> initializer,
  std::optional<AstTypePtr> type
): kind(kind), externName(externName), initializer(initializer), type(type) {}

bool AstDeclNode::operator==(const AstNode &other) const {
  if (auto node = dynamic_cast<const AstDeclNode*>(&other)) {
    return 
      node->kind == this->kind &&
      node->externName == this->externName &&
      node->initializer == this->initializer &&
      node->type == this->type;
  }
  return false;
}

