#pragma once

#include <memory>

namespace parser {
  class AstType {
  public:
    virtual bool operator==(const AstType &other) const = 0;
  };

  typedef std::shared_ptr<AstType> AstTypePtr;
}


