#pragma once

#include <parser/ast/body.hpp>
#include <parser/ast/types/function.hpp>
#include <parser/ast/expression.hpp>

namespace parser {
  class AstFunctionExpression final : public AstExpression {
  private:
    AstFunctionTypePtr type;
    AstBody body;
  public:
    AstFunctionExpression(
      AstFunctionTypePtr type,
      AstBody body
    );
    bool operator==(const AstExpression &other) const override;
  };
}
