#pragma once

#include <memory>
#include <optional>
#include <parser/ast/type.hpp>
#include <vector>

namespace parser {
  class AstFunctionTypeArg {
  private:
    std::string_view name;
    AstTypePtr type;

  public:
    bool operator==(const AstFunctionTypeArg &other) const;
  };

  class AstFunctionType final : public AstType {
  private:
    std::optional<AstTypePtr> returns;
    std::vector<AstFunctionTypeArg> args;
  public:
    AstFunctionType(
      std::optional<AstTypePtr> returns,
      std::vector<AstFunctionTypeArg> args
    );
    bool operator==(const AstType &other) const override;
  };

  typedef std::shared_ptr<AstFunctionType> AstFunctionTypePtr;
}
