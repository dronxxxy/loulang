#pragma once

#include <parser/ast/path.hpp>
#include <parser/ast/type.hpp>

namespace parser {
  class AstPathType final : public AstType {
  private:
    AstPath path;

  public:
    AstPathType(AstPath path);
    bool operator==(const AstType &other) const override;
  };
}


