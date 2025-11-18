#pragma once

#include <parser/ast/node.hpp>
#include <parser/ast/expression.hpp>
#include <parser/ast/type.hpp>
#include <string>
#include <optional>

namespace parser {
  enum class AstDeclKind { Meta, Constant, Final, Var };

  class AstDeclNode final : public AstNode {
  private:
    AstDeclKind kind;
    std::optional<std::string> externName;
    std::optional<AstExpressionPtr> initializer;
    std::optional<AstTypePtr> type;

  public:
    AstDeclNode(
      AstDeclKind kind,
      std::optional<std::string> externName,
      std::optional<AstExpressionPtr> initializer,
      std::optional<AstTypePtr> type
    );
    bool operator==(const AstNode &other) const override;
  };
}

