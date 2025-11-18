#pragma once

#include <memory>

namespace parser {
  class AstExpression {
  public:
    virtual bool operator==(const AstExpression &other) const = 0;
  };

  typedef std::shared_ptr<AstExpression> AstExpressionPtr;
}

