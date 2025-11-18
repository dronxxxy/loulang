#pragma once

#include <parser/ast/node.hpp>
#include <lexer/lexer.hpp>
#include <logger/logger.hpp>
#include <memory>
#include <optional>

namespace parser {
  class Parser {
  private:
    std::shared_ptr<logger::Logger> logger;
    lexer::Lexer lexer;
  
  public:
    Parser(
      std::shared_ptr<logger::Logger> logger,
      lexer::Lexer &&lexer
    );
    
    std::optional<std::shared_ptr<AstNode>> parseNext();
  };
}
