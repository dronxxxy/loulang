#pragma once

#include <parser/ast/node.hpp>
#include <variant>
#include <vector>

namespace parser {
  class AstStatement {
    // TODO
  };

  typedef std::shared_ptr<AstStatement> AstStatementPtr;
  
  typedef std::variant<AstNodePtr, AstStatementPtr> AstBodyChild;

  class AstBody {
  private:
    std::vector<AstBodyChild> children;
  public:
    AstBody(std::vector<AstBodyChild> &&children);
    bool operator==(const AstBody &other) const;
  };
}
