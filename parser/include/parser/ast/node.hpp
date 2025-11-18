#pragma once

#include <memory>

namespace parser {
  class AstNode {
  public:
    virtual bool operator==(const AstNode &other) const = 0;
  };

  typedef std::shared_ptr<AstNode> AstNodePtr;
}
